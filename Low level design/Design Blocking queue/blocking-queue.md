# Blocking Queue

A Blocking Queue is a thread-safe queue where:

- If the queue is empty, a consumer thread waits (blocks) until an item is available.
- If the queue is full (in a bounded queue), a producer thread waits (blocks) until space becomes available.

It is commonly used for producer-consumer problems in multithreaded applications.

## Producer-Consumer Flow

```text
Producer
   ↓
[ Blocking Queue ]
   ↓
Consumer
```

## When the Queue Is Full

Suppose the queue capacity is 3:

```text
Queue: [A] [B] [C]   ← FULL
```

If the producer tries to add D:

```text
Producer → put(D)
              ↓
           BLOCKS
```

It waits until the consumer removes something.

## When the Queue Is Empty

Similarly:

```text
Queue: [ ] [ ] [ ]
       EMPTY
```

If the consumer calls:

```text
Consumer → take()
              ↓
           BLOCKS
```

It waits until the producer adds an item.

