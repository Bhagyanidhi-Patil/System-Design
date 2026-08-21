Imagine two threads accessing the same variable:

```cpp
int balance = 100;
```

Both threads modify balance at the same time. This can cause a race condition.

A mutex ensures that only one thread enters the critical section at a time.

## 1. std::mutex

**This is the basic mutex.**

```cpp
mutex mtx;


mtx.lock();


// critical section
balance += 100;


mtx.unlock();
```

**Think:**

**🔒 Lock → do work → 🔓 Unlock**

**Problem**

If something goes wrong between `lock()` and `unlock()`, the mutex might never be unlocked.

For example:

```cpp
mtx.lock();


someFunction();  // throws exception


mtx.unlock();   // never reached
```

> **Note:** So manually using `lock()`/`unlock()` is generally avoided in modern C++.

## 2. std::lock_guard

**This is the simplest and safest way to lock a mutex.**

```cpp
mutex mtx;


void update() {
     lock_guard<mutex> lock(mtx);


     balance += 100;
}
```

When this line executes:

```cpp
lock_guard<mutex> lock(mtx);
```

the mutex is automatically locked.

When lock goes out of scope:
```
function starts
      ↓
lock_guard created
      ↓
mutex locked 🔒
      ↓
critical section
      ↓
function ends
      ↓
lock_guard destroyed
      ↓
mutex unlocked 🔓
```
**This is called RAII.**

**Use `lock_guard` when:**

You simply want:

"Lock this mutex for this entire scope."

## 3. std::unique_lock

**unique_lock is more flexible than lock_guard.**

```cpp
unique_lock<mutex> lock(mtx);
```

It automatically locks the mutex, just like `lock_guard`.

But it gives you additional control.

For example:

```cpp
unique_lock<mutex> lock(mtx);


lock.unlock();


// do something without the mutex


lock.lock();


// critical section again
```

You cannot do this conveniently with `lock_guard`.

**Why is `unique_lock` important?**

Because condition variables use `unique_lock`.

For example:

```cpp
unique_lock<mutex> lock(mtx);


cv.wait(lock, [&]() {
     return !q.empty();
});
```

Remember what `wait()` needs to do:
```
Lock mutex
    ↓
Condition false?
    ↓
Release mutex 🔓
    ↓
Sleep 😴
    ↓
Someone calls notify()
    ↓
Wake up
    ↓
Lock mutex again 🔒
    ↓
Check condition
```
**`unique_lock` allows the condition variable to temporarily unlock and relock the mutex.**

## 4. std::shared_mutex

**This is used when you have many readers but relatively few writers.**

For example:
```
Thread 1 → READ
Thread 2 → READ
Thread 3 → READ
Thread 4 → READ
```
All four can read simultaneously.

But if somebody wants to write:

`Thread 1 → WRITE`

then everybody else must wait.

You need:

```cpp
#include <shared_mutex>


shared_mutex mtx;
```

For reading:

```cpp
shared_lock<shared_mutex> lock(mtx);


cout << data;
```

For writing:

```cpp
unique_lock<shared_mutex> lock(mtx);


data = 100;
```

So:
```
                 shared_mutex
                        │
          ┌─────────┴─────────┐
          ↓                   ↓
        READ                WRITE
          ↓                   ↓
 shared_lock          unique_lock
          ↓                   ↓
Multiple readers      Only one writer
allowed               at a time
```
## 5. std::recursive_mutex

Sometimes a thread needs to lock the same mutex multiple times.

**Normal mutex:**

```cpp
mutex mtx;


mtx.lock();
mtx.lock();  // ❌ problematic
```

The thread is essentially waiting for itself.

**A `recursive_mutex` allows this:**

```cpp
recursive_mutex mtx;


mtx.lock();
mtx.lock();  // ✅ allowed



mtx.unlock();
mtx.unlock();
```

It keeps track of how many times the same thread has locked it.

However, don't use `recursive_mutex` just because you're having trouble designing the locking logic. Usually there's a cleaner solution.
```cpp
recursive_mutex mtx;


void A() {
     lock_guard<recursive_mutex> lock(mtx);
     B();
}


void B() {
     lock_guard<recursive_mutex> lock(mtx);
}
```

A() locks the mutex and then calls B().

B() tries to lock the same mutex from the same thread.
```
With normal mutex → ❌ deadlock.

With recursive_mutex → ✅ allowed.
```

## 6. std::timed_mutex

Sometimes you don't want to wait forever for a mutex.

You can try to acquire it for a limited amount of time.

```cpp
timed_mutex mtx;


if (mtx.try_lock_for(chrono::seconds(2))) {


     // got the lock


     mtx.unlock();
}
else {
     cout << "Could not acquire lock";
}
```

**Meaning:**

"Try to get the lock for 2 seconds. If you can't, give up."