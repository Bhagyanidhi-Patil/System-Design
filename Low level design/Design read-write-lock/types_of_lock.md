### 1. std::lock_guard
Simplest and fastest.
- Locks the mutex immediately when created.
- Unlocks automatically when it goes out of scope.
- Cannot manually unlock or relock.
- Cannot transfer ownership.

#### Example

```
#include <iostream>
#include <mutex>

using namespace std;

mutex mtx;

void print()
{
    lock_guard<mutex> lock(mtx); // Lock acquired

    cout << "Critical Section" << endl;

} // Automatically unlocked here
```

### 2. std::unique_lock

- unique_lock is more flexible.

It can
- lock later
- unlock before scope ends
- relock
- transfer ownership
- work with condition variables

#### Example
```
#include <iostream>
#include <mutex>

using namespace std;

mutex mtx;

void fun()
{
    unique_lock<mutex> lock(mtx);

    cout << "Locked" << endl;

    lock.unlock();        // Manual unlock

    cout << "Doing non-critical work" << endl;

    lock.lock();          // Lock again

    cout << "Locked again" << endl;

} // Unlocks if still locked
```

### 3. std::shared_lock
- std::shared_lock is used with a shared mutex (std::shared_mutex) to allow multiple threads to read the same data simultaneously, while still preventing writes during those reads.

Think of it this way:

`lock_guard / unique_lock` → Exclusive lock (only one thread at a time)
`shared_lock → Shared lock` (many readers can access together)

std::shared_mutex, which supports two kinds of locks:
`Shared lock (read lock)` → Many readers allowed.
`Exclusive lock (write lock)` → Only one writer, no readers.

```
#include <iostream>
#include <thread>
#include <shared_mutex>

using namespace std;

shared_mutex smtx;
int value = 100;

void reader(int id)
{
    shared_lock<shared_mutex> lock(smtx);

    cout << "Reader " << id
         << " reads " << value << endl;
}

void writer()
{
    unique_lock<shared_mutex> lock(smtx);

    value++;

    cout << "Writer updated value to "
         << value << endl;
}

int main()
{
    thread t1(reader,1);
    thread t2(reader,2);
    thread t3(writer);

    t1.join();
    t2.join();
    t3.join();
}
```

---

### If `shared_lock` allows multiple read and single write, then why not to use shared_lock? Why to use unique_lock ?
The reason is because the interviewer is asking you to `design a Read-Write Lock`, not simply use one.

`Why not use shared_lock in our implementation?`
- Because shared_lock is just an RAII wrapper. It does not implement the locking algorithm itself.

---

### Condition variable 
-  condition variable is a synchronization primitive that allows one thread to wait until another thread notifies it that some condition has become true.
- It's commonly used in Producer-Consumer, Thread Pool, Blocking Queue, and many multithreaded designs.


#### The most common syntax is:
```
cv.wait(lock, [] {
    return ready;
});
```
or, if ready is a local variable:
```
cv.wait(lock, [&] {
    return ready;
});
```

#### `General Syntax`
`cv.wait(lock, predicate);`
where

`predicate`
is simply a function that returns true or false.

```
[]      -> What variables does the lambda capture?
()      -> What parameters does it take?
{}      -> What code does it execute?
```

`cv.wait(lock, predicate) waits until the predicate becomes true.`
Think of it like this:
```
cv.wait(lock, [&] {
    return condition;
});
```
is equivalent to:
```
while (!condition)
{
    cv.wait(lock);
}
```

`General Syntax:`
```
[capture](parameters) -> return_type {
    // function body
};
```
#### 1. [] — Capture List

It tells the lambda which variables from the surrounding scope it can use.

Empty capture []

Use when you don't need any local variables.
```
auto greet = []() {
    cout << "Hello";
};

greet();
```

##### Capture by reference [&]

Use when you want to access and possibly modify local variables.
```
int x = 10;

auto print = [&]() {
    cout << x;
};

print();
```

##### Capture by value [=]

Use when you want a copy of local variables.
```
int x = 10;

auto print = [=]() {
    cout << x;
};

print();
```
#### 2. () — Parameters

This is exactly like a normal function's parameter list.

##### No parameters:
```
[]() {
    cout << "Hello";
}
```

#### One parameter:
```
[](int x) {
    return x * 2;
}
```

#### Two parameters:
```
[](int a, int b) {
    return a + b;
}
```