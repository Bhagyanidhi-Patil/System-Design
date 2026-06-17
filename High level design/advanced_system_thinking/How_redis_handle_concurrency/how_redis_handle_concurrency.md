Redis handles concurrency primarily by processing commands sequentially (in queue) in a single-threaded event loop, which makes individual operations atomic and avoids traditional locking.


# 1. The Concurrency Problem

Imagine 1,000 clients are connected to Redis.

Two users simultaneously execute:

```text
Client A: INCR counter
Client B: INCR counter
```

Without proper concurrency control, both could read:

```text
counter = 5
```

and both write:

```text
counter = 6
```

Result:

```text
Expected: 7
Actual: 6
```

This is called a **race condition**.

---

# 2. Redis's Core Idea

Redis uses a **single command execution thread**.

Think of Redis like a bank teller:

```text
Customers arrive simultaneously

A ----\
B ----- > Queue --> Teller --> Process one by one
C ----/
```

Even if thousands of clients send requests at the same time:

```text
INCR counter
SET user:1 ...
GET order:5
LPUSH queue ...
```

Redis places them into an event queue and executes:

```text
1st command
2nd command
3rd command
...
```

one at a time.

Therefore:

```text
No two commands modify data simultaneously.
```
> **Redis receives requests from thousands of clients concurrently, but instead of executing them simultaneously, it places them in an event loop and processes commands one at a time. Since Redis stores data entirely in RAM memory and its data structures are highly optimized, most commands complete in microseconds. As a result, even though command execution is single-threaded, Redis can handle hundreds of thousands to millions of operations per second with very low latency.**

---

# 3. Example: Concurrent Increment

Suppose:

```text
counter = 5
```

Two clients send:

```text
INCR counter
```

at exactly the same moment.

Internally:

```text
Queue:

1. Client A -> INCR
2. Client B -> INCR
```

Execution:

```text
counter = 5

A executes:
counter = 6

B executes:
counter = 7
```

Result:

```text
counter = 7
```

No race condition.

---

# 4. Why No Locks Are Needed

In many databases:

```text
Thread 1
Thread 2
Thread 3
```

all access memory simultaneously.

So they need:

```text
Mutexes
Read Locks
Write Locks
Semaphores
```

Example:

```text
lock(counter)
counter++
unlock(counter)
```

Redis avoids most of this because:

```text
Only one thread modifies data.
```

Therefore:

```text
No lock contention
No deadlocks
No context switching overhead
```

This is one reason Redis is extremely fast.

---

# 5. What Is Atomic in Redis?

A single Redis command is atomic.

Example:

```text
SET key value
INCR counter
HSET user name John
```

Each command completes fully before the next begins.

## Atomic Example

```text
INCR likes
```

Internally:

```text
Read value
Add 1
Write value
```

Even though there are multiple internal steps, Redis guarantees:

```text
No other command can run in between.
```

So:

```text
INCR = Atomic
```

# 6. Where Problems Can Still Occur

A common interview question is:

**"Redis is single-threaded. Does that mean race conditions are impossible?"**

The answer is **No**.

Redis guarantees that **each individual command is atomic**, but when your business logic requires multiple commands, race conditions can still occur.

Consider a banking example:

```redis
GET balance
IF balance > 100
DECRBY balance 100
```

Suppose the account balance is:

```text
balance = 200
```

Now two clients try to withdraw ₹100 simultaneously.

### Timeline

#### Client A

```redis
GET balance
```

Redis returns:

```text
200
```

#### Client B

Before Client A performs the withdrawal, Client B also executes:

```redis
GET balance
```

Redis returns:

```text
200
```

Now both clients believe there is enough balance.

#### Client A

```redis
DECRBY balance 100
```

Balance becomes:

```text
100
```

#### Client B

```redis
DECRBY balance 100
```

Balance becomes:

```text
0
```

Both withdrawals succeeded because both clients made decisions based on the same old value.

This is called a **race condition at the application level**.

Notice that Redis executed every command one by one correctly. The problem occurred because the entire business operation required multiple commands.

---

# 7. Solution: Transactions

To prevent other clients from inserting commands in the middle of a logical operation, Redis provides transactions.

Commands:

```redis
MULTI
...
EXEC
```

Example:

```redis
MULTI
INCR counter
INCR counter
EXEC
```

When Redis receives `MULTI`, it starts recording commands instead of executing them immediately.

```text
Transaction Queue

INCR counter
INCR counter
```

When `EXEC` is called, Redis executes all queued commands sequentially as one transaction.

Execution:

```text
Client A Transaction
--------------------
INCR counter
INCR counter
--------------------
```

During execution, Redis does not allow commands from other clients to be interleaved.

## Problem Without Transaction

Suppose the account balance is:

```text
balance = 200
```

Two clients try to withdraw ₹100 simultaneously.

### Client A

```redis
GET balance
```

Redis returns:

```text
200
```

### Client B

```redis
GET balance
```

Redis also returns:

```text
200
```

Both clients now make the same decision:

```text
200 > 100
```

So both proceed with the withdrawal:

```redis
DECRBY balance 100
```

Final balance becomes:

```text
0
```

Both withdrawals succeeded.

The problem is that the business decision:

```text
IF balance > 100
```

was made outside Redis, based on a value that may have become stale.

---

## What MULTI/EXEC Does

Suppose Client A executes:

```redis
MULTI
DECRBY balance 100
INCR withdrawal_count
EXEC
```

And Client B executes:

```redis
MULTI
DECRBY balance 100
INCR withdrawal_count
EXEC
```

Redis queues the commands for each transaction.

```text
Client A Transaction
--------------------
DECRBY balance 100
INCR withdrawal_count
--------------------

Client B Transaction
--------------------
DECRBY balance 100
INCR withdrawal_count
--------------------
```

When `EXEC` is called, Redis executes all commands in a transaction together.

Execution order becomes:

```text
A: DECRBY balance 100
A: INCR withdrawal_count

B: DECRBY balance 100
B: INCR withdrawal_count
```

No interleaving occurs.

Redis will never execute:

```text
A: DECRBY balance 100
B: DECRBY balance 100
A: INCR withdrawal_count
B: INCR withdrawal_count
```

This is the guarantee provided by `MULTI/EXEC`.

---

## But Does MULTI/EXEC Solve the Withdrawal Problem?

No.

Suppose both clients first execute:

```redis
GET balance
```

and both receive:

```text
200
```

Then both start transactions.

### Client A

```redis
MULTI
DECRBY balance 100
EXEC
```

### Client B

```redis
MULTI
DECRBY balance 100
EXEC
```

Execution:

```text
Initial balance = 200

A transaction:
balance = 100

B transaction:
balance = 0
```

Both withdrawals still succeed.

Why?

Because `MULTI/EXEC` only guarantees:

> Commands inside a transaction execute together without interruption.

It does **not** guarantee:

> The data read before `MULTI` is still unchanged when `EXEC` runs.

Therefore, `MULTI/EXEC` prevents command interleaving but does not prevent stale reads. For read-modify-write operations, Redis uses `WATCH` (optimistic locking) or Lua scripts for true concurrency safety.

# 9. Lua Scripts – The Best Way to Handle Complex Concurrency in Redis

A common problem in Redis is when a business operation requires multiple steps:

```text id="6dyukw"
1. Read a value
2. Make a decision
3. Update the value
```

For example, in a wallet system:

```text id="f4nlx7"
If balance >= 100
    Deduct 100
```

A naive implementation might be:

```redis id="kiv1if"
GET balance

IF balance >= 100

DECRBY balance 100
```

The problem is that multiple clients can read the same balance before either performs the deduction, leading to race conditions.

---

## How Lua Scripts Solve This

Lua scripts solve concurrency problems because Redis treats the entire script as a single atomic operation.

Instead of executing:

```text id="a9h3zp"
GET
CHECK
UPDATE
```

as separate commands that can be interleaved with other clients' requests, Redis executes the entire script from start to finish before processing the next request.

## Without Lua

Suppose:

```text id="h7kp2m"
balance = 200
```

Two clients attempt to withdraw ₹100 simultaneously.

```text id="s5d8cw"
Client A: GET balance -> 200

Client B: GET balance -> 200

Client A: DECRBY balance 100

Client B: DECRBY balance 100
```

Both clients read the same balance before either updates it.

This can lead to race conditions because both make decisions using stale data.

---

## With Lua

The entire business logic is sent to Redis as one script:

```lua id="x4nm7q"
local balance = redis.call('GET', KEYS[1])

if tonumber(balance) >= 100 then
    redis.call('DECRBY', KEYS[1], 100)
end
```

Redis treats this entire script as:

```text id="2v4n8t"
Operation 1
```

not as:

```text id="f8w2jd"
GET
CHECK
DECRBY
```

---

## What Happens Internally?

Suppose two clients send the same Lua script at the same time.

Internally Redis creates a queue:

```text id="q6r3pk"
1. Client A Lua Script
2. Client B Lua Script
```

Execution becomes:

```text id="j1m7cf"
Run Script A completely
-----------------------
GET balance
CHECK balance
DECRBY balance
-----------------------

Run Script B completely
-----------------------
GET balance
CHECK balance
DECRBY balance
-----------------------
```

No other command can run between the steps of Script A.

---

## Even If Multiple Threads Send Requests Simultaneously

Application side:

```text id="y8v5mx"
Thread A ---> Lua Script

Thread B ---> Lua Script
```

Redis execution:

```text id="n4z7hk"
Thread A Script (100% complete)

Then

Thread B Script (100% complete)
```

Redis will never execute:

```text id="r3c9sw"
Thread A: GET

Thread B: GET

Thread A: DECRBY

Thread B: DECRBY
```

because the entire Lua script is treated as one atomic unit.

---

## Why Lua Scripts Are Popular

Lua scripts are commonly used for:

* Wallet balance deduction
* Inventory reservation
* Rate limiting
* Distributed locks

