# Two-Phase Commit (2PC)

Two-Phase Commit (2PC) is a distributed transaction protocol used to ensure that multiple databases/services either all commit a transaction or all roll it back, maintaining atomicity across systems.

## The Problem 2PC Solves

Imagine a banking transaction:

- Account A (Database 1): Deduct ₹1000
- Account B (Database 2): Add ₹1000

What if:

- Database 1 commits successfully
- Database 2 crashes before committing

Now money disappears.

We need a way to guarantee:

- Either both operations succeed or both fail.

This is where 2PC comes in.

## Coordinator

The coordinator manages the transaction.

Examples:

- Transaction Manager
- Orchestrator
- Master node

## Participants

The systems involved in the transaction.

Examples:

- Database A
- Database B
- Service X
- Service Y

## Two Phases

### Phase 1: Prepare Phase (Voting)

Coordinator asks every participant:

> "Can you commit this transaction?"

Step-by-step:

```text
Coordinator
  |
  +----> DB1 : PREPARE?
  |
  +----> DB2 : PREPARE?
```

Each participant:

- Executes transaction locally
- Does NOT commit yet
- Writes changes to a log
- Locks affected data
- Responds:
  - YES (Ready)
  - NO (Cannot commit)

Example:

- DB1 -> YES
- DB2 -> YES

### Phase 2: Commit Phase

#### Case 1: Everyone votes YES

Coordinator sends:

- COMMIT

to all participants.

```text
Coordinator
  |
  +----> DB1 : COMMIT
  |
  +----> DB2 : COMMIT
```

Participants:

- Commit changes permanently
- Release locks
- Send ACK

Result:

- Transaction Success

#### Case 2: Anyone votes NO

Suppose:

- DB1 -> YES
- DB2 -> NO

Coordinator sends:

- ROLLBACK

to everyone.

```text
Coordinator
  |
  +----> DB1 : ROLLBACK
  |
  +----> DB2 : ROLLBACK
```

Result:

- Transaction Failed
- No partial updates occur.

## Example: E-Commerce Order

Suppose placing an order involves:

- Inventory Service
- Payment Service

Without 2PC:

- Payment charged
- Inventory update failed

Customer loses money but order isn't placed.

With 2PC:

- Phase 1:
  - Payment: Ready
  - Inventory: Ready
- Phase 2:
  - Commit both
  - or
  - Rollback both

Consistency is maintained.

## Why Participants Write Logs

Before replying YES, each participant writes:

`PREPARED TRANSACTION T123`

to durable storage.

If the machine crashes:

- Restart
- Read log
- Continue transaction

This prevents losing transaction state.

## Example: Atomic Transfer

Suppose:

- Transfer ₹1000
- Account A → Account B

Prepare Phase:

Bank A:

- Can deduct ₹1000
- READY

Bank B:

- Can add ₹1000
- READY

Commit Phase:

Coordinator:

- COMMIT

Both banks commit.

Atomic transfer achieved.

## Advantages of 2PC

- Strong Consistency
  - All participants see the same result.
- Atomic Transactions
  - All-or-nothing behavior.
- Easy Conceptually
  - Simple protocol to understand and implement.

## Problems with 2PC

This is the most important interview discussion.

### 1. Blocking Protocol

Suppose:

- All participants voted YES

Then coordinator crashes.

Participants don't know:

- Commit?
- Rollback?

They must wait.

This is called blocking.

### 2. Long-Lived Locks

During Prepare Phase:

- Rows remain locked
- Other transactions may be blocked
- Higher latency

Result:

- Lower throughput

### 3. Poor Scalability

Every transaction requires:

- Prepare message
- Commit message
- Acknowledgements

- Many network round trips
- Performance degrades as participants increase

### 4. Coordinator is a Bottleneck

The coordinator can become a single point of contention.



## Why is 2PC Rare in Modern Microservices?

Because:

- Distributed transactions are expensive

2PC causes:

- Blocking
- Lock contention
- Reduced availability

Modern systems prioritize:

- Availability
- Scalability
- Fault tolerance

over strict consistency.

Therefore many companies use:

- Event-driven architecture
- Saga Pattern
- Outbox Pattern
- Eventual Consistency

instead of 2PC.

Eventual Consistency

instead of 2PC.

## 2PL vs 2PC Comparison

|           | 2PL                                   | 2PC                                                   |
| --------- | ------------------------------------- | ----------------------------------------------------- |
| Full Form | Two-Phase Locking                     | Two-Phase Commit                                      |
| Solves    | Concurrency control                   | Distributed transaction coordination                  |
| Goal      | Prevent conflicting concurrent access | Ensure all systems commit or rollback together        |
| Used In   | Databases, transaction managers       | Distributed databases, microservices                  |
| Main Idea | Lock data before modifying it         | Get agreement from all participants before committing |


