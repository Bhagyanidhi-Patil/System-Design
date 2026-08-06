# Consistency

Consistency is the guarantee that `all users or nodes see data according to a defined consistency model` after a write operation.

Consistency answers one simple question:

> If I write new data to the database, when will every user see that new data?

## Replica example

```text
Client
   |
Write "Hello"
   |
Primary Database
 /   |   \
/    |    \
Replica1 Replica2 Replica3
```

Now imagine a user updates their profile picture.

**Question:** Will everyone immediately see the new profile picture?

That depends on the consistency model.

## Strong Consistency

Strong consistency guarantees that once a `write operation is successfully completed, every subsequent read from any replica returns the most recent value.`

- After a successful write, every future read returns the latest value.
- No user can ever see stale data.

### Example

- Old Name = Alice
- User changes: Alice → Bob

Immediately after update:

- Primary = Bob
- Replica1 = Bob
- Replica2 = Bob
- Replica3 = Bob

Only after every replica has been updated does the write succeed.

Now:

- User A reads → Bob
- User B reads → Bob
- User C reads → Bob

Everyone sees the same data.

### Timeline

1. 10:00:00 — Update Name = Bob
2. Primary updates
3. Replica1 updates
4. Replica2 updates
5. Replica3 updates
6. Write succeeds
7. Every read returns Bob

### Advantages

- ✅ No stale reads
- ✅ Simple reasoning
- ✅ Perfect data correctness

### Disadvantages

- Every write has to wait for all replicas.
- Latency grows with distant replicas.

Example:

- India
- USA
- Europe

If replica updates take:

- Primary: 50 ms
- Replica1: 60 ms
- Replica2: 120 ms
- Replica3: 200 ms

Then the write completes only after 200 ms.

## Eventual Consistency

Eventual consistency guarantees that `after a write, all replicas will eventually converge to the same value`, but some reads may return stale data during propagation.

- The primary updates immediately.
- Replicas are updated later.
- The write can return success before every replica is updated.

### Example

- Primary = Bob
- Replica1 = Alice
- Replica2 = Alice
- Replica3 = Alice

If a read goes to Replica2, the user still sees Alice.

After a few seconds:

- Replica2 = Bob

Now everyone sees Bob.

Eventually, all replicas converge.

### Timeline

1. 10:00:00 — Update Bob
2. Primary updated
3. Write succeeds
4. Replica1 updated (10:00:01)
5. Replica2 updated (10:00:03)
6. Replica3 updated (10:00:05)

Between 10:00:00 and 10:00:05, some users may see Alice.

After 10:00:05, everyone sees Bob.

### Why use Eventual Consistency?

- It is faster.
- You do not wait for all replicas before returning success.
- The primary updates first and replicas sync later.

## Use cases

### Strong consistency

Bank transfer example:

- Initial balance: ₹5000
- Transfer: ₹3000
- New balance: ₹2000

If one server still shows ₹5000 and you withdraw again, the same money could be spent twice.

Banks require strong consistency to prevent this.

### Eventual consistency

Real-life example:

- Instagram uploads
- Primary server updates immediately
- Nearby users may see content instantly
- Distant users may see it after a short delay

This is acceptable for many social and caching workloads.
