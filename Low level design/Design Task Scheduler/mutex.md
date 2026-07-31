# Mutex Behavior in Scheduler

## If multiple functions use the same mutex with lock_guard or unique_lock, can only one thread execute any of those critical sections at a time, or can one thread be in addTask() while another is in run()?

- The mutex (mtx) is shared across the entire Scheduler object, not tied to a particular function.

- lock_guard and unique_lock are just different ways of locking the same mutex.

Think of the mutex as a room with one key

**Imagine this:**

```text
                 Scheduler

             +----------------+
             | mutex (mtx) 🔑 |
             +----------------+
```

- `addTask()`  ---- uses this key

- `run()`      ---- uses the SAME key

There is only one key.

- Whichever thread gets the key first enters the critical section.

- No other thread can acquire it until the first thread releases it.

## Example

Suppose

Thread 1 (Main Thread)

```cpp
scheduler.addTask(task);

void addTask(...) {
    lock_guard<mutex> lock(mtx);   // acquires mtx
    ...
}
```

At the same time,

Thread 2 (Worker Thread)

```cpp
run();

unique_lock<mutex> lock(mtx);
```

What happens?

### Time
```
Main Thread
-------------
lock_guard(mtx)
✓ acquires mutex

Worker Thread
-------------
unique_lock(mtx)

❌ blocked
waiting...
```

The worker cannot continue because the same mutex is already locked.

## Does only one thread enter the function?

No.

- A mutex does not lock a function.

- It locks a resource (the shared data).

**For example:**

```cpp
void addTask() {
    cout << "Before lock\n";

    lock_guard<mutex> lock(mtx);

    cout << "Inside lock\n";
}
```

Two threads can both enter addTask().

### Example:
```
Thread 1
---------
Before lock

acquires mutex

Inside lock

Thread 2
---------
Before lock

tries lock

(waiting)
```

**Notice:**

- Both threads entered the function.
- Only one thread entered the critical section (the code protected by the mutex).

## If a mutex is locked in one function, does it prevent other threads from entering any other function that uses the same mutex, even if the mutex is locked in a different function?

### Case 1: Same mutex (your scheduler)

```cpp
class Scheduler {
    mutex mtx;

    void addTask() {
        lock_guard<mutex> lock(mtx);
        ...
    }

    void run() {
        unique_lock<mutex> lock(mtx);
        ...
    }
};
```

There is one mutex (mtx).

Both functions use the same mutex.

So if addTask() locks mtx:

**Thread 1**
---------
addTask()
lock(mtx)
✓ acquired

**Thread 2**
---------
run()
lock(mtx)
❌ waits

Here, run() must wait because it's trying to acquire the same mutex.

### Case 2: Different mutexes

```cpp
mutex mtx1;
mutex mtx2;

void functionA() {
    lock_guard<mutex> lock(mtx1);
}

void functionB() {
    lock_guard<mutex> lock(mtx2);
}
```

Now suppose:

**Thread 1**
---------
functionA()
lock(mtx1)
✓

**Thread 2**
---------
functionB()
lock(mtx2)
✓

Both threads run at the same time because they are locking different mutexes.

They don't interfere with each other.

### Case 3: One function doesn't lock anything

```cpp
mutex mtx;

void addTask() {
    lock_guard<mutex> lock(mtx);
}

void printHello() {
    cout << "Hello\n";
}
```

Suppose:

**Thread 1**
---------
addTask()
lock(mtx)

**Thread 2**
---------
printHello()

Can printHello() run?

Yes!

Because printHello() never tries to lock mtx.

The mutex has no effect on it.

