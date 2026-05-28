# Distributed Locking

Distributed locking is a mechanism used in distributed systems to ensure that only one machine, thread, or process can access or modify a shared resource at a time.

It is like a normal mutex or lock in programming, but it works across:

- multiple servers
- multiple containers
- multiple microservices

## Why We Need It

Suppose you have 3 application servers, all connected to the same database.

### Problem Example

Imagine an e-commerce system with only 1 item left in stock.

Two servers receive purchase requests simultaneously:

- Server A → Buy item
- Server B → Buy item

Without locking:

- Both may sell the same item
- This creates duplicate orders
- The state becomes inconsistent
- Race conditions occur

## Distributed Lock Solution

Before modifying stock:

- Acquire lock
- Only one server succeeds
- Other servers wait or fail

### Simple Flow Diagram

```text
          +----------------+
Server A →|                |
          | Distributed    |
Server B →| Lock Service   |
          |                |
Server C →|                |
          +----------------+
```

Only one gets:

- `LOCK ACQUIRED`

Others get:

- `LOCK DENIED`

## Common Use Cases

1. **Inventory Management**
   - Prevent overselling.
2. **Scheduled Jobs (Cron Jobs)**
   - Ensure only one server executes the same job.
3. **Leader Election**
   - Choose one leader node.
4. **Payment Processing**
   - Prevent duplicate transactions.

## How Distributed Lock Works

A distributed lock usually relies on shared storage or a coordination service such as:

- Redis
- ZooKeeper
- etcd
- Database row locking

## Redis Distributed Lock

This is a very common interview topic.

### Basic Idea

Use a Redis key such as:

`lock:order_123`

A server tries:

```text
SET lock:order_123 unique_value NX EX 30
```

Meaning:

- `NX` → only set if the key does not exist
- `EX 30` → auto expire after 30 seconds

Example:

- Server A executes `SET lock:item123 abc NX EX 30` and succeeds.
- Server B executes `SET lock:item123 xyz NX EX 30` and fails because the key already exists.

### Why Expiration Is Important

If a server crashes after taking the lock, the lock should not remain forever.

Without expiration:

- A deadlock can occur.

With expiration:

- The lock auto-expires and becomes available again.

## Important Problem: Accidental Unlock

What if Server A releases Server B's lock accidentally?

### Example Timeline

1. Server A acquires the lock.
   - Redis: `lock:item123 = A_UUID` (expires in 30 sec)
2. A becomes slow due to GC pause, network delay, CPU freeze, or long DB query.
3. The lock expires automatically.
4. Server B acquires the lock with `SET lock:item123 B_UUID NX EX 30`.

Now B owns the lock.

A is still alive and later tries to release its lock. If A executes `DEL lock:item123`, it will accidentally delete B's lock.

### Safe Release

Store a unique ID in the lock value:

`lock:item123 = serverA_randomUUID`

Before deleting, verify ownership:

```text
IF lock_value == my_unique_id
THEN DEL lock:item123
```

This ensures only the lock owner can release it.

## Important Characteristics

A good distributed lock should provide:

- **Mutual exclusion**: only one owner at a time.
- **Deadlock free**: locks eventually get released.
- **Fault tolerance**: works even if nodes fail.

## Challenges in Distributed Locking

1. **Network failures**
   - A server may think it still owns the lock while the lock has expired elsewhere.
2. **Clock issues**
   - Time-based assumptions can cause incorrect behavior.

---

### But Important Interview Insight

Redlock is controversial.

Some engineers argue:
```
Not safe for critical distributed consensus
```

### For highly critical systems:
```
ZooKeeper
etcd
are often preferred.
ZooKeeper / etcd Locks
```

These systems provide:
```
strong consistency
leader election
distributed coordination
```
**More reliable than simple Redis locks.** 