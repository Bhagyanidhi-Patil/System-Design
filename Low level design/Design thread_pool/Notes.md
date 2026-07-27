## In the constructor, during the first iteration of the for loop (i = 0), workers.emplace_back() creates a thread that immediately enters the while(true) loop. Since this is an infinite loop, how does the for loop continue to i = 1 and create the remaining worker threads? Shouldn't the execution get stuck in the first while(true)?

The key is that **`emplace_back()` creates a new thread**. The new thread executes the lambda, **not the main thread**.

Let's go through it step by step.

---

## First iteration (`i = 0`)

The main thread executes:

```cpp
workers.emplace_back([this]() {
    while (true) {
        ...
    }
});
```

### What happens here?

* A **new worker thread** (say **Worker 1**) is created.
* **Worker 1** starts executing the lambda:

```cpp
while (true) {
    ...
}
```

However, **the main thread does not enter this `while(true)` loop**.

Instead, `emplace_back()` returns immediately, allowing the main thread to continue executing the `for` loop.

Current state:

```text
Main Thread
    |
    +---- Creates Worker 1
              |
              +---- Runs while(true) independently
```

---

## Second iteration (`i = 1`)

The main thread continues the `for` loop:

```cpp
i = 1;
workers.emplace_back(...);
```

A second worker thread is created.

Current state:

```text
Main Thread
    |
    +---- Worker 1 --> while(true)
    |
    +---- Worker 2 --> while(true)
```

---

## Third iteration (`i = 2`)

Again,

```cpp
workers.emplace_back(...);
```

creates **Worker 3**.

Current state:

```text
Main Thread
    |
    +---- Worker 1 --> while(true)
    |
    +---- Worker 2 --> while(true)
    |
    +---- Worker 3 --> while(true)
```

---

## What happens next?

The constructor finishes.

The main thread continues with:

```cpp
for (int i = 1; i <= 10; i++)
    pool.submit(...);
```

Meanwhile, all three worker threads are already running and are blocked inside:

```cpp
cv.wait(...);
```

waiting for tasks to be submitted.

---

## Think of it like hiring employees

Imagine you're a manager.

```text
for (3 workers)
    hire(worker);
```

When you hire **Worker 1**:

* Worker 1 immediately starts working.
* **You don't become Worker 1.**
* You continue hiring Worker 2.

Then:

* Worker 2 starts working.
* You continue hiring Worker 3.

After hiring all three workers, **you** continue doing your own work.

This is exactly how `std::thread` behaves.

---

## Why doesn't `while(true)` block the `for` loop?

Because the `while(true)` executes in a **different thread**.

```text
Main Thread                     Worker 1
-----------                     --------
emplace_back()
                                starts
returns immediately             while(true)
i++
```

The main thread never enters the worker thread's infinite loop.

---

## Timeline

```text
Main Thread
------------
i = 0
Create Worker 1 ----------------------------+
                                            |
i = 1                                       |
Create Worker 2 ----------------------+     |
                                       |     |
i = 2                                  |     |
Create Worker 3 ------------------+    |     |
                                  |    |     |
Constructor ends                  |    |     |
submit()                          |    |     |
                                  |    |     |
Worker 1 ------------------------>| while(true) -> wait()
Worker 2 -----------------------------> while(true) -> wait()
Worker 3 ----------------------------------> while(true) -> wait()
```

### Conclusion

The constructor does **not** get stuck in the first `while(true)` because that loop runs in a **separate worker thread**. The main thread immediately returns from `emplace_back()`, continues the `for` loop, creates the remaining worker threads, and then proceeds to execute the rest of the program.
