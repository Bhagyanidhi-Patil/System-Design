# Two-Phase Locking (2PL)

Two-Phase Locking (2PL) is a database concurrency-control technique that ensures transactions don't corrupt data when multiple users access it at the same time.

## Simple Analogy

Imagine a library with books.

Before reading or editing a book, you must reserve (lock) it.

- If you're only reading → take a shared lock (others can also read).
- If you're editing → take an exclusive lock (nobody else can read or edit).

2PL says:

### Phase 1: Growing Phase 🔒

- You can acquire locks, but you cannot release any.

Example:

- Lock Account A
- Lock Account B
- Lock Customer Record

### Phase 2: Shrinking Phase 🔓

Once you release your first lock:

- You can only release locks
- You cannot acquire any new locks

Example:

- Unlock Account A
- Unlock Account B
- Unlock Customer Record

So the transaction looks like:

```text
Acquire Lock A
Acquire Lock B
Acquire Lock C
-----------------
Release Lock A
Release Lock B
Release Lock C
```

You never go back to acquiring locks after releasing one.

## Why do we need it?

### Transaction T1

Suppose T1 wants to update Account A.

Growing phase:

- T1 acquires lock(A)
- T1 reads/updates A

T1 is still in its growing phase because it hasn't released any lock yet.

### Transaction T2

At the same time, T2 also wants to update Account A.

- T2 tries to acquire lock(A)
- But A is already locked by T1.

So:

- T2 waits

T2 is trying to acquire a lock, but the database lock manager doesn't grant it because T1 already owns the lock.

## What "acquire lock" really means

When we say:

- Acquire lock(A)

it doesn't mean:

- Lock is always granted

It means:

- Request lock(A)

The database checks:

- Is someone else holding a conflicting lock?
  - No → grant lock.
  - Yes → make transaction wait.

## Timeline Example

Time 1:

- T1 requests X-lock(A)
- ✓ Granted

Time 2:

- T2 requests X-lock(A)
- ✗ Must wait

Time 3:

- T1 commits
- T1 releases lock(A)

Time 4:

- T2 gets lock(A)
- ✓ Granted

## Strict 2PL

Strict 2PL is a stricter version of Two-Phase Locking that most real databases use.

## The main drawbacks of 2PL

Regular 2PL only says:

Growing phase:

- acquire locks

Shrinking phase:

- release locks

Example:

```text
lock(A)
lock(B)

update A
update B

unlock(A)   <-- shrinking starts
unlock(B)

commit
```

This follows 2PL because once unlocking starts, no new locks are acquired.

### The problem with regular 2PL

Suppose:

- T1
  - lock(A)
  - A = A + 100
  - unlock(A)   <-- released before commit

- T2
  - lock(A)
  - read(A)

Now T2 sees the updated value from T1.

But what if T1 later fails?

- ROLLBACK

T2 has already read data that never should have existed.

This is called a dirty read.

## Deadlocks ⚠️

The biggest problem.

Example:

```text
T1:
  lock(A)
  wants lock(B)

T2:
  lock(B)
  wants lock(A)
```

Now:

- T1 waits for T2
- T2 waits for T1

Neither can proceed.

This is called a deadlock.

## Reduced Concurrency

Locks force transactions to wait.

Example:

- T1:
  - lock(A)
  - update A

- T2:
  - wants A
  - waits...

Even if T2 is ready to run, it cannot proceed until T1 releases the lock.

As contention increases, throughput drops.

## Cascading Waits / Lock Contention

Example:

- T1 holds A
- T2 waits for A
- T3 waits for T2's result
- T4 waits for T3

One blocked transaction can cause a chain of waiting transactions.

This is sometimes called a convoy effect.

## Strict 2PL Rule

- Hold all exclusive (write) locks until COMMIT or ROLLBACK.
- Do not release write locks early.

Example:

```text
lock(A)
lock(B)

update A
update B

COMMIT

unlock(A)
unlock(B)
```

Notice:

```text
update A
unlock A   <-- NOT ALLOWED
```

before commit.

## 2PL vs 2PC Comparison

|           | 2PL                                   | 2PC                                                   |
| --------- | ------------------------------------- | ----------------------------------------------------- |
| Full Form | Two-Phase Locking                     | Two-Phase Commit                                      |
| Solves    | Concurrency control                   | Distributed transaction coordination                  |
| Goal      | Prevent conflicting concurrent access | Ensure all systems commit or rollback together        |
| Used In   | Databases, transaction managers       | Distributed databases, microservices                  |
| Main Idea | Lock data before modifying it         | Get agreement from all participants before committing |
