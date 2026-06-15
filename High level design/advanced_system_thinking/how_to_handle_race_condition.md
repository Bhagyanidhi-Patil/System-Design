# How do you handle race conditions?
### What is a Race Condition?

A **race condition** occurs when multiple processes, threads, services, or users access and modify the same data concurrently, and the final result depends on the timing or order of execution.

### Example

Suppose a bank account has a balance of **₹1000**.

Two withdrawal requests arrive simultaneously:

* User A withdraws ₹700
* User B withdraws ₹500

Without proper synchronization:

```text
Balance = 1000

Thread A reads 1000
Thread B reads 1000

Thread A updates balance = 300
Thread B updates balance = 500

Final balance = 500
```

The correct balance should be:

```text
1000 - 700 - 500 = -200
```

or one of the transactions should fail due to insufficient funds.

This inconsistency occurs because both threads read and update the shared data independently, without coordination. Such incorrect behavior is known as a **race condition**.

---

## Solution 1: Pessimistic Locking 

**Pessimistic Locking** is a concurrency control technique where a transaction **locks a resource before reading or updating it**, assuming that conflicts are likely to happen.

### Example: Inventory Management

Suppose an e-commerce website has a product with **10 items in stock**.

Two customers try to buy the same product at the same time.

#### Step 1: Transaction A Acquires a Lock

```sql
SELECT *
FROM inventory
WHERE product_id = 1
FOR UPDATE;
```

`FOR UPDATE` places an exclusive lock on that row.

Current stock = **10**.

---

#### Step 2: Transaction B Tries to Access the Same Row

```sql
SELECT *
FROM inventory
WHERE product_id = 1
FOR UPDATE;
```

Transaction B must **wait** because Transaction A already holds the lock.

---

#### Step 3: Transaction A Updates Stock

```sql
UPDATE inventory
SET stock = stock - 1
WHERE product_id = 1;
```

Stock becomes **9**.

---

#### Step 4: Transaction A Commits

```sql
COMMIT;
```

The lock is released.

---

#### Step 5: Transaction B Continues

Now Transaction B can acquire the lock and read the latest stock value (**9**), preventing incorrect updates and ensuring data consistency.

---

## Solution 2: Optimistic Concurrency Control (OCC)

**Optimistic Concurrency Control (OCC)** assumes that conflicts between transactions are rare. Instead of locking data before updating it, OCC allows multiple transactions to read the same data concurrently and detects conflicts only when an update occurs.

### Add a Version Field

Consider the following inventory record:

```text
Product:
id = 1
stock = 10
version = 5
```

The `version` column tracks how many times the row has been modified.

---

### Update Operation

When updating the stock, include the current version in the `WHERE` clause:

```sql
UPDATE inventory
SET stock = 9,
    version = 6
WHERE id = 1
  AND version = 5;
```

The update succeeds only if the row's version is still `5`.

---

### What Happens If Another Transaction Modified the Row?

If another process has already updated the record and changed the version, the condition:

```sql
WHERE version = 5
```

will no longer match.

As a result:

```text
Rows affected = 0
```

This indicates a concurrency conflict.

The application should:

1. Re-read the latest data.
2. Apply the business logic again.
3. Retry the update.

---

### Example

#### Initial State

```text
stock = 10
version = 5
```

#### Thread A Reads

```text
stock = 10
version = 5
```

#### Thread B Reads

```text
stock = 10
version = 5
```

Both threads have the same version number.

---

#### Thread A Updates Successfully

```sql
UPDATE inventory
SET stock = 9,
    version = 6
WHERE id = 1
  AND version = 5;
```

Result:

```text
stock = 9
version = 6
```

---

#### Thread B Tries to Update

```sql
UPDATE inventory
SET stock = 8,
    version = 6
WHERE id = 1
  AND version = 5;
```

This fails because the current version is now `6`, not `5`.

```text
Rows affected = 0
```

Thread B detects the conflict and must retry.

---

## Solution 3: Atomic Database Operations

Instead of performing separate **Read → Modify → Write** steps, use a single atomic database statement that performs the update directly in the database.

This eliminates race conditions because the database executes the operation as one indivisible unit.

### Bad Approach: Read → Modify → Write

```python
stock = get_stock()
stock -= 1
save(stock)
```

#### Problem

Suppose the current stock is **10**.

```text
Thread A reads stock = 10
Thread B reads stock = 10

Thread A updates stock = 9
Thread B updates stock = 9

Final stock = 9 ❌
Expected stock = 8
```

This is a classic **Lost Update Problem** caused by concurrent access.

---

### Good Approach: Atomic Update

```sql
UPDATE inventory
SET stock = stock - 1
WHERE stock > 0;
```

The database performs:

1. Check that stock is greater than zero.
2. Decrement stock.
3. Commit the change.

All in a single atomic operation.

---

### Why It Works

Consider two concurrent requests when:

```text
stock = 1
```

#### Thread A

```sql
UPDATE inventory
SET stock = stock - 1
WHERE stock > 0;
```

Result:

```text
Rows affected = 1
stock = 0
```

#### Thread B

Runs the same statement immediately afterward:

```sql
UPDATE inventory
SET stock = stock - 1
WHERE stock > 0;
```

Result:

```text
Rows affected = 0
```

Because `stock > 0` is no longer true.

The database prevents overselling automatically.

---

### Checking Success

Most applications verify the number of affected rows:

```sql
UPDATE inventory
SET stock = stock - 1
WHERE id = 1
  AND stock > 0;
```

If:

```text
Rows affected = 1
```

Purchase succeeded.

If:

```text
Rows affected = 0
```

Item is out of stock or another transaction already consumed the inventory.

---

## Solution 4: Transactions

A **Transaction** is a sequence of database operations that are executed as a single unit of work. Transactions follow the **ACID** properties:

* **Atomicity** – All operations succeed or none do.
* **Consistency** – Data remains valid before and after the transaction.
* **Isolation** – Concurrent transactions do not interfere with each other.
* **Durability** – Once committed, changes are permanently stored.

Transactions are commonly used when multiple related operations must either all succeed or all fail together.

---

### Example: Money Transfer

Suppose we want to transfer **$100** from Account A to Account B.

#### Begin Transaction

```sql
BEGIN;
```

#### Read Accounts

```sql
SELECT balance FROM accounts WHERE id = 'A';
SELECT balance FROM accounts WHERE id = 'B';
```

#### Debit Account A

```sql
UPDATE accounts
SET balance = balance - 100
WHERE id = 'A';
```

#### Credit Account B

```sql
UPDATE accounts
SET balance = balance + 100
WHERE id = 'B';
```

#### Commit

```sql
COMMIT;
```

Both updates become permanent together.

---

### What If Something Fails?

Imagine Account A is debited successfully, but the system crashes before Account B is credited.

Without a transaction:

```text
Account A = -100
Account B = unchanged
```

Money is effectively lost.

With a transaction:

```sql
ROLLBACK;
```

The database restores all affected rows to their original state.

```text
Account A = original balance
Account B = original balance
```

No partial updates occur.

---

### Transaction Flow

```text
Begin Transaction
       ↓
Read Data
       ↓
Perform Updates
       ↓
Success?
   /      \
 Yes      No
  |         |
Commit   Rollback
```

---

### Example: Bank Transfer

Initial State:

```text
Account A = $1000
Account B = $500
```

Transfer $100:

```text
Account A = $900
Account B = $600
```

Both changes are committed together.

If any step fails:

```text
Account A = $1000
Account B = $500
```

The database rolls back the transaction.

---

## Solution 5: Distributed Locking

When an application runs on multiple servers, **local locks are not sufficient** because each server has its own memory and lock state.

### The Problem

Consider an inventory service running on three servers:

```text
Server A
Server B
Server C
```

All three servers can receive requests to update the same product inventory.

If each server uses only local synchronization mechanisms (such as Java `synchronized`, `ReentrantLock`, or in-memory mutexes), the locks are visible only within that server.

Example:

```text
Server A acquires local lock
Server B acquires local lock
Server C acquires local lock
```

All three servers may update the same inventory simultaneously, leading to race conditions and inconsistent data.

---

### Solution: Distributed Lock

A **Distributed Lock** is a lock shared across all servers in the system.

Before processing a critical operation:

1. Acquire the distributed lock.
2. Perform the business logic.
3. Release the lock.

Flow:

```text
Acquire Distributed Lock
           ↓
     Process Request
           ↓
    Release Lock
```

Only one server can hold the lock at a time.

---

### Example

Suppose we want to update inventory for Product 123.

Lock Key:

```text
inventory:product:123
```

#### Server A

```text
Acquire lock → Success
Update inventory
Release lock
```

#### Server B

```text
Acquire lock → Wait / Fail
```

#### Server C

```text
Acquire lock → Wait / Fail
```

Only Server A can modify the inventory until the lock is released.

---

### Common Technologies

#### Redis + Redlock

One of the most popular distributed locking approaches.

```text
SET inventory:product:123 unique_id NX PX 30000
```

Where:

* `NX` = Create lock only if it doesn't exist.
* `PX` = Lock expiration time in milliseconds.

Benefits:

* Fast
* Easy to implement
* Suitable for many web applications

---

#### ZooKeeper

Provides strong consistency and distributed coordination.

Commonly used in:

* Hadoop ecosystems
* Large-scale distributed systems

Benefits:

* Reliable leader election
* Strong coordination guarantees

---

#### etcd

A distributed key-value store widely used in cloud-native systems.

Used by:

* Kubernetes
* Service discovery systems
* Configuration management

Benefits:

* Strong consistency
* Fault tolerance

---

#### Consul

Provides:

* Distributed locking
* Service discovery
* Health checks

Often used in microservice architectures.

---

### Example Workflow

Without Distributed Lock:

```text
Server A reads stock = 10
Server B reads stock = 10

Server A updates stock = 9
Server B updates stock = 9

Final stock = 9 ❌
Expected stock = 8
```

With Distributed Lock:

```text
Server A acquires lock
Server B waits

Server A updates stock = 9
Server A releases lock

Server B acquires lock
Server B updates stock = 8

Final stock = 8 ✅
```

---
