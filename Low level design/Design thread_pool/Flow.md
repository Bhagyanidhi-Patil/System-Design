## Flow : 

```

ThreadPool pool(3)
        │
        ▼
Create 3 worker threads
        │
        ▼
Each thread enters while(true)
        │
        ▼
Queue empty?
        │
      Yes
        │
        ▼
Sleep in cv.wait()
        │
        ▼
submit()
        │
        ▼
Task pushed into queue
        │
        ▼
notify_one()
        │
        ▼
One worker wakes
        │
        ▼
Take task
        │
        ▼
Execute task
        │
        ▼
Loop again
        │
        ├── Queue not empty → take another task
        │
        └── Queue empty → sleep
```

## CV condition explaination
`In this code:`

```cpp
cv.wait(lock, [this]() {
    return stop || !tasks.empty();
});
```

`stop` means:

```cpp
stop == true
```

So the predicate is actually equivalent to:

```cpp
cv.wait(lock, [this]() {
    return (stop == true) || (!tasks.empty());
});
```

Let's see all cases.

### Case 1: `stop = false`, queue is empty

```cpp
stop = false;
tasks.empty() = true;

return false || false;
```

**Result:**

```cpp
false
```

➡️ The worker **continues waiting**.

---

### Case 2: `stop = false`, queue has tasks

```cpp
stop = false;
tasks.empty() = false;

return false || true;
```

**Result:**

```cpp
true
```

➡️ The worker **wakes up** and executes a task.

---

### Case 3: `stop = true`, queue is empty

```cpp
stop = true;
tasks.empty() = true;

return true || false;
```

**Result:**

```cpp
true
```

➡️ The worker **wakes up**. Then it executes:

```cpp
if (stop && tasks.empty())
    return;
```

Since both conditions are true, the worker thread **exits**.

---

### Case 4: `stop = true`, queue has tasks

```cpp
stop = true;
tasks.empty() = false;

return true || true;
```

**Result:**

```cpp
true
```

➡️ The worker **wakes up**, processes the remaining tasks, and only exits after the queue becomes empty.


## Destructor explaination 

```cpp
~ThreadPool() {
    {
        lock_guard<mutex> lock(mtx);
        stop = true;
    }

    cv.notify_all();

    for (auto &t : workers)
        t.join();
}
```

### Step-by-step Explanation

#### Step 1: Acquire the mutex

```cpp
lock_guard<mutex> lock(mtx);
```

The mutex is locked so that no worker thread can access or modify shared data (`stop` and `tasks`) while it is being updated.

---

#### Step 2: Set the stop flag

```cpp
stop = true;
```

This tells all worker threads that the thread pool is shutting down.

---

#### Step 3: Release the mutex

When `lock_guard` goes out of scope, the mutex is automatically unlocked.

---

#### Step 4: Wake up all worker threads

```cpp
cv.notify_all();
```

Any workers sleeping inside

```cpp
cv.wait(lock, [this]() {
    return stop || !tasks.empty();
});
```

are awakened.

Since `stop == true`, the predicate becomes `true`, so all workers continue execution.

---

#### Step 5: Worker behavior after waking

Each worker checks:

```cpp
if (stop && tasks.empty())
    return;
```

There are two possibilities:

**Case 1: Queue is empty**

```text
stop = true
tasks.empty() = true
```

The condition is true, so the worker thread exits immediately.

---

**Case 2: Queue still contains tasks**

```text
stop = true
tasks.empty() = false
```

The condition is false, so the worker removes a task from the queue and executes it.

After finishing, it loops back, checks the queue again, and continues processing any remaining tasks.

Once the queue becomes empty, the condition

```cpp
stop && tasks.empty()
```

becomes true, and the worker exits.

---

#### Step 6: Wait for all workers to finish

```cpp
for (auto &t : workers)
    t.join();
```

`join()` blocks the destructor until every worker thread has exited.

Only after all worker threads finish does the destructor return, ensuring that the thread pool shuts down cleanly without leaving any threads running.

- `You never call the destructor yourself; C++ calls it automatically.`
- `When execution reaches: return 0; the local variable pool goes out of scope. Since pool is a stack (automatic) object, C++ automatically calls : ~ThreadPool();`

**So the execution looks like this:**
```
main()
  │
  ▼
ThreadPool pool(3);
  │
  ▼
Constructor called
  │
  ├── stop = false
  │
  ├── Create Worker 1 ──► Starts immediately ──► Sleeps in cv.wait()
  │
  ├── Create Worker 2 ──► Starts immediately ──► Sleeps in cv.wait()
  │
  └── Create Worker 3 ──► Starts immediately ──► Sleeps in cv.wait()
  │
  ▼
Constructor finishes
  │
  ▼
submit() is called
  │
  ▼
Task added to queue
  │
  ▼
notify_one()
  │
  ▼
One worker wakes up and executes the task
   │
   ▼
return 0;
   │
   ▼
pool goes out of scope
   │
   ▼
~ThreadPool()            ← Destructor called automatically
   │
   ▼
stop = true
notify_all()
join() all threads
   │
   ▼
Program exits
```