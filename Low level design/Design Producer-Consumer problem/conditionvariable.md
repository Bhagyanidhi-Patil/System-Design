# Why do We Need a Condition Variable?

Imagine you have two threads:

* **Producer Thread** → Produces data
* **Consumer Thread** → Consumes data

Suppose initially there is no data.

## Without Condition Variable

The consumer keeps checking:

```cpp
while(queue.empty())
{
    // keep checking
}
```

This is called **Busy Waiting (Spin Waiting)**.

```text
Consumer

Is queue empty?
Yes

Is queue empty?
Yes

Is queue empty?
Yes

Is queue empty?
Yes
```

The CPU continuously runs this loop, wasting CPU time.

---

Instead, we want the consumer to behave like this:

```text
Consumer:
"I have nothing to do.
Wake me up when the producer adds something."
```

That's exactly what **`std::condition_variable`** does.

It allows a thread to:

* Sleep efficiently
* Wake up only when another thread notifies it

---

### Real-Life Example

Imagine ordering food in a restaurant.

You place your order.

Instead of asking every second:

```text
Is my food ready?
Is my food ready?
Is my food ready?
```

You simply sit and wait.

Later, the waiter comes and tells you:

```text
Sir, your order is ready.
```

Here,

* **Waiter** → Producer
* **Customer** → Consumer
* **Waiter's notification** → `condition_variable.notify_one()`

---

## Components

A condition variable works together with three things:

```text
mutex
condition_variable
predicate (condition)
```

Example:

```cpp
mutex mtx;
condition_variable cv;
queue<int> q;
```

---

# Workflow

Suppose the queue is empty.

Consumer starts:

```text
Consumer

Lock mutex

Queue empty?

Yes

Go to sleep
```

Now Producer starts:

```text
Producer

Lock mutex

Insert data

Unlock mutex

Notify consumer
```

Consumer wakes up:

```text
Consumer

Gets mutex

Consumes data

Unlocks mutex
```

---

## Visualization

Initially:

```text
Queue

Empty

Producer ------------>

                 Queue

Consumer
     |
     |
     v

Sleeping
```

Producer inserts data:

```text
Queue

10
20
30

Producer

notify_one()
```

Consumer wakes:

```text
Consumer

Remove 10

Continue
```

---

## Syntax

### Declaration

```cpp
std::mutex mtx;
std::condition_variable cv;
```

### Waiting Thread

```cpp
unique_lock<mutex> lock(mtx);

cv.wait(lock);
```

### Wake One Thread

```cpp
cv.notify_one();
```

### Wake All Waiting Threads

```cpp
cv.notify_all();
```

---

## Why `unique_lock` and not `lock_guard`?

A condition variable must temporarily release the mutex while waiting.

Only `unique_lock` supports automatic:

```text
unlock()
lock()
```

`lock_guard` cannot unlock before its scope ends.

Therefore:

```text
condition_variable
        +
unique_lock
```

always go together.

---

## How `wait()` Works Internally

Suppose you call:

```cpp
cv.wait(lock);
```

Internally it performs:

```text
1. Unlock mutex

2. Put thread to sleep

3. Wait for notification

4. Wake up

5. Lock mutex again

6. Continue execution
```

**Important:** While the thread is sleeping, the mutex is **not held**, allowing other threads to acquire it.

---

## Example

```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

mutex mtx;
condition_variable cv;

bool ready = false;

void consumer()
{
    unique_lock<mutex> lock(mtx);

    cv.wait(lock);

    cout << "Consumer starts working\n";
}

void producer()
{
    this_thread::sleep_for(2s);

    unique_lock<mutex> lock(mtx);

    ready = true;

    cout << "Producer finished\n";

    lock.unlock();

    cv.notify_one();
}

int main()
{
    thread t1(consumer);
    thread t2(producer);

    t1.join();
    t2.join();
}
```

### Output

```text
Producer finished
Consumer starts working
```

---

## But There Is a Problem!

Suppose the notification comes before the consumer starts waiting.

Timeline:

```text
Producer

notify_one()

---------------------------->

Consumer

wait()
```

The consumer misses the notification.

Result:

```text
Consumer sleeps forever.
```

This is called a **Missed Notification**.

---

## Correct Way

Always wait using a predicate.

```cpp
cv.wait(lock, []{
    return ready;
});
```

This is equivalent to:

```cpp
while (!ready)
{
    cv.wait(lock);
}
```

Now, even if the notification came earlier:

```text
ready == true
```

the consumer won't sleep.

---

## Complete Correct Example

```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

mutex mtx;
condition_variable cv;

bool ready = false;

void consumer()
{
    unique_lock<mutex> lock(mtx);

    cv.wait(lock, []{
        return ready;
    });

    cout << "Consumer running\n";
}

void producer()
{
    {
        lock_guard<mutex> lock(mtx);

        ready = true;
    }

    cv.notify_one();
}

int main()
{
    thread t1(consumer);
    thread t2(producer);

    t1.join();
    t2.join();
}
```

---

## `notify_one()`

Suppose three consumers are waiting:

```text
Consumer1
Consumer2
Consumer3
```

Producer calls:

```cpp
cv.notify_one();
```

Only **one** waiting thread wakes up:

```text
Consumer1  <-- wakes

Consumer2 sleeping

Consumer3 sleeping
```

---

## `notify_all()`

```cpp
cv.notify_all();
```

All waiting threads wake up:

```text
Consumer1

Consumer2

Consumer3

All wake
```

However, only one thread acquires the mutex first.

The remaining threads wait until the mutex becomes available.

---

## Spurious Wakeup

Sometimes a thread wakes up **without any notification**.

```text
Thread sleeping

↓

Random wake-up

↓

Condition still false
```

This is known as a **Spurious Wakeup**.

That's why the recommended pattern is:

```cpp
cv.wait(lock, [] {
    return ready;
});
```

or

```cpp
while (!ready)
{
    cv.wait(lock);
}
```

Never write:

```cpp
if (!ready)
{
    cv.wait(lock);   // ❌ Incorrect
}
```

because after waking up, the condition may still be false.

---

## Rule to Remember

The predicate passed to `cv.wait()` should answer this question:

> **"Is it okay for this thread to continue now?"**

* If the answer is **Yes**, return `true`.
* If the answer is **No**, return `false`.

For example:

```cpp
cv.wait(lock, [] {
    return ready;
});
```

This means:

> **"Continue only when `ready` becomes `true`."**

You'll commonly see predicates like these:

```cpp
cv.wait(lock, [] { return ready; });
cv.wait(lock, [] { return !queue.empty(); });
cv.wait(lock, [] { return count > 0; });
cv.wait(lock, [] { return finished; });
```

In every case, the predicate returns **`true` only when the waiting thread should stop waiting and proceed with execution**. Until then, `cv.wait()` keeps the thread waiting.

---

## Understanding `cv.wait(lock, predicate)`

The general syntax is:

```cpp
cv.wait(lock, predicate);
```

It means:

> **Wait until the `predicate` becomes `true`.**

Internally, it behaves like this:

```cpp
while (!predicate())
{
    cv.wait(lock);
}
```

So the thread keeps waiting as long as the predicate is `false`.

---

## Example 1: `ready`

```cpp
cv.wait(lock, [] {
    return ready;
});
```

This means:

> **Wait until `ready` becomes `true`.**

Execution:

```cpp
ready = false
```

* Predicate returns `false` → Thread sleeps.

Later,

```cpp
ready = true;
cv.notify_one();
```

* Predicate returns `true` → Thread wakes up and continues.

---

## Example 2: Queue

```cpp
cv.wait(lock, [] {
    return !queue.empty();
});
```

This means:

> **Wait until the queue is NOT empty.**

Execution:

Initially,

```cpp
queue = { }
```

Then,

```cpp
queue.empty()      // true
!queue.empty()     // false
```

Since the predicate is `false`, the consumer waits.

Later, the producer pushes an item:

```cpp
queue.push(10);
cv.notify_one();
```

Now,

```cpp
queue.empty()      // false
!queue.empty()     // true
```

The predicate is now `true`, so the consumer wakes up and starts consuming the item.

---

## Easy Trick to Remember

Whenever you see:

```cpp
cv.wait(lock, [] {
    return CONDITION;
});
```

Read it as:

> **"Wait until `CONDITION` becomes true."**

Examples:

```cpp
cv.wait(lock, [] { return ready; });
```

➡️ Wait until `ready == true`.

```cpp
cv.wait(lock, [] { return !queue.empty(); });
```

➡️ Wait until the queue has at least one element.

```cpp
cv.wait(lock, [] { return count > 0; });
```

➡️ Wait until `count` becomes greater than `0`.

```cpp
cv.wait(lock, [] { return finished; });
```

➡️ Wait until `finished == true`.
