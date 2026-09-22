# Thread-Safe Queue

## What is a Thread-Safe Queue?

A thread-safe queue is a queue designed so that multiple threads can safely access it at the same time.

For example:

```text
Thread 1 ──┐
Thread 2 ──┼──> Thread-Safe Queue
Thread 3 ──┘
```

We typically use a mutex to protect the queue.

```cpp
void push(int value) {
    lock_guard<mutex> lock(mtx);
    q.push(value);
}
```

- Only one thread can enter the critical section at a time.
- `Multiple threads can access the queue" does NOT mean "multiple threads can modify the queue at the exact same time.`

- `A thread-safe queue means multiple threads are allowed to use the queue safely, but the actual modification is usually serialized using a mutex.`
```
Thread-safe queue = queue + mutex

Blocking queue = queue + mutex + condition_variable
```

## What happens if the queue is empty?

Suppose:

```text
Queue = [ ]
```

Consumer calls:

```text
pop()
```

A basic thread-safe queue can simply say:

```text
Queue empty
     ↓
return false
```

The thread does not have to wait.

So thread-safe mainly answers:

"Can multiple threads access this queue safely?"

## What is a Blocking Queue?

A blocking queue is a thread-safe queue that additionally provides waiting behavior.

Suppose:

```text
Queue = [ ]
```

Consumer calls:

```text
pop()
```

Instead of returning immediately:

```text
Queue empty
     ↓
Consumer waits ⏸️
```

Later, producer does:

```text
push(10)
```

Then:

```text
Producer
   ↓
push(10)
   ↓
Queue = [10]
   ↓
notify consumer
   ↓
Consumer wakes up
   ↓
pop() → 10
```

So a blocking queue answers two questions:

"Can multiple threads access me safely?"

and

"What should happen if the operation cannot proceed right now?"

It makes the thread wait.

## The biggest difference

This is the part you should remember.

### Thread-safe queue

```text
Consumer
    |
    | pop()
    ↓
Queue empty?
    |
    YES
    ↓
Return false
```

### Blocking queue

```text
Consumer
    |
    | pop()
    ↓
Queue empty?
    |
    YES
    ↓
WAIT ⏸️
    |
    ↓
Producer adds item
    |
    ↓
WAKE UP
    |
    ↓
Return item
```

## What about a full queue?

This is relevant when we have a bounded queue, meaning the queue has a maximum capacity.

Suppose capacity = 3:

```text
[10] [20] [30]
             ↑
            FULL
```

### Thread-safe queue

If producer tries:

```cpp
push(40);
```

the implementation could simply:

```text
Queue full
    ↓
return false / error
```

### Blocking queue

It can instead:

```text
Queue full
    ↓
Producer waits ⏸️
    ↓
Consumer removes 10
    ↓
Space available
    ↓
Producer wakes up
    ↓
Adds 40
```