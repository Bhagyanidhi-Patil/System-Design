# Problem Statement

Design a **distributed key value store / system** that supports the following operation (key value store - NOSQL DB):

- `PUT(key, value)`
- `GET(key)`
- `DELETE(key)`

Example:

```text
PUT("user:123", "John")

GET("user:123")
â†’ "John"

DELETE("user:123")
```

## Functional Requirements

- Store arbitrary key-value pairs.
- Read by key.
- Write by key.
- Delete by key.

## Non-Functional Requirements

- Horizontally scalable.
- Highly available.
- Fault tolerant.
- Low latency.
- Durable.
- Handle billions of keys.

## Examples in industry

- Redis
- Amazon DynamoDB
- Apache Cassandra
- etcd
- Riak

## High-Level Architecture

```text
                  Client
                    |
                    v
             Any KV Node
            (Coordinator)
                    |
                    v
          Consistent Hashing
                    |
                    v
        -----------------------
        |          |          |
        v          v          v
      Node A    Node B    Node C
       Primary   Replica   Replica
```

Each node stores a subset of data.

### The challenge:

Which node should store a key?

## Data Partitioning (Sharding)

Without sharding:

- Node A stores everything

Problems:

- Storage limit
- CPU bottleneck
- Single point of failure

### Naive Hashing

`node = hash(key) % N`

Example:

- `N = 4`
- `hash(user123) % 4 = 2`

Store on Node 2

Works initially.

## Problem During Scaling

Suppose:

- `4 nodes â†’ 5 nodes`

Now:

- `hash(key) % 5`

Almost every key moves.

This causes:

- Massive data migration
- Cache invalidation
- Network overhead

## Consistent Hashing

Imagine a circular ring:

```text
0 ---------------------> MAX_HASH
 \                       /
  \                     /
   ---------------------
```

Nodes are placed on the ring.

Example:

- Node A
- Node B
- Node C

```text
          A
      /       \
    C           B
```

Keys are also hashed onto the ring.

### Placement Rule

A key belongs to the first node clockwise.

Example:

- Key K1
- `K1 ---> Node B`

### Advantages

When a node is added:

- Node D

Only nearby keys move.

Instead of:

- 100% migration

We get:

- ~1/N migration

Huge improvement.

## Virtual Nodes (VNodes)

Real systems rarely place one point per node.

Instead:

- Node A:
  - A1 A2 A3 A4
- Node B:
  - B1 B2 B3 B4
- Node C:
  - C1 C2 C3 C4

Spread across ring.

### Benefits:

- Better Load Balancing

Without vnodes:

- A owns 70%
- B owns 20%
- C owns 10%

Bad distribution.

With vnodes:

- A ≈ 33%
- B ≈ 33%
- C ≈ 33%

### Easier Rebalancing

When a server fails:

- Only vnode ranges move
- instead of entire server ranges.

## Replication

### Without Replication

Suppose we have only one node:

`user:123 -> John`

```text
        ┌─────────┐
        │ Node A  │
        └─────────┘
```

User writes:

`PUT(user:123, John)`

Data is stored only on Node A.

### Problem

If Node A crashes:

```text
        ┌─────────┐
        │ Node A  │ ❌
        └─────────┘
```

The data is gone.

`user:123 -> John`

is lost forever.

This is called a single point of failure.

### With Replication

Suppose:

Replication Factor (RF) = 3

Meaning:

Store 3 copies of every piece of data.

#### Write Flow

User writes:

`PUT(user:123, John)`

The system stores it on:

```text
        ┌─────────┐
        │ Node A  │  Primary
        └─────────┘

        ┌─────────┐
        │ Node B  │  Replica
        └─────────┘

        ┌─────────┐
        │ Node C  │  Replica
        └─────────┘
```

Now all three nodes contain:

`user:123 -> John`

What if Node A dies?

```text
        ┌─────────┐
        │ Node A  │ ❌
        └─────────┘

        ┌─────────┐
        │ Node B  │ ✓
        └─────────┘

        ┌─────────┐
        │ Node C  │ ✓
        └─────────┘
```

Can we still read the data?

Yes.

Node B and Node C still have:

`user:123 -> John`

So requests continue to work.

#### Why RF = 3 is Common

With 3 copies:

- A
- B
- C

you can tolerate losing one node:

- A ❌
- B ✓
- C ✓

and still have data.

In many systems, you can even survive multiple failures depending on quorum settings.

### Replica nodes means nodes on different hash ring or nodes of same hash ring ?

There is usually one consistent hashing ring for the cluster.

Example:

```text
             A2
        /         \
     B1             C2

  A1                 B2

     C1          A3
        \      /
            B3
```

All virtual nodes (A1, A2, B1, etc.) belong to the same ring.

Suppose:

`hash(user:123)`

lands at:

`A2`

and:

`RF = 3`

Then we walk clockwise and pick the next distinct physical nodes.

- `A2  -> Node A  (Primary)`
- `B2  -> Node B  (Replica)`
- `C1  -> Node C  (Replica)`

Data is stored on:

- Node A
- Node B
- Node C

### What we do NOT do

Not:

- Ring 1 -> Primary
- Ring 2 -> Replica
- Ring 3 -> Replica

There aren't separate hash rings for replicas in the typical Dynamo/Cassandra design.

### Mental Model
```
- One Ring
  ↓
- Many Virtual Nodes
  ↓
- Map to Physical Nodes
  ↓
- Replication picks multiple physical nodes
  ↓
- Store copies there
```
`A1, A2, A3 are NOT partitions of data`

They are virtual nodes (positions on the ring) that belong to physical Node A.

Physical Node A
- A1
- A2
- A3

Physical Node B
- B1
- B2
- B3

Physical Node C
- C1
- C2
- C3

Think of A1, A2, A3 as:

`"Three seats owned by Node A on the hash ring."`

They are not replicas.

---

### Write Flow

Suppose:

`PUT(user:123, "John")`

Step 1: Find the partition
```
`hash("user:123")`
      ↓
`Partition 5`
```
Using consistent hashing or partitioning.

Step 2: Send to replicas

Assume:

Replication Factor (N) = 3

Data is stored on:

- Node A
- Node B
- Node C

Step 3: Acknowledge write

Different choices:

- Strong consistency

  Wait for majority:

  - A ✓
  - B ✓
  - C pending

  Return success

  Example:

  `W = 2`

  Need 2 acknowledgements.

- High availability

  Return after one replica:

  - A ✓

  Return success immediately

  Faster but weaker consistency.

### Read Flow

Suppose:

`GET(user:123)`

Step 1: Find partition
```
`hash("user:123")`
      ↓
`Partition 5`
```
Same as write.

Step 2: Query replicas

Replicas:

- Node A
- Node B
- Node C

#### Option 1: Read from one replica

```text
Client
  ↓
Node A
  ↓
Return value
```

Very fast.

But Node A may be stale.

#### Option 2: Read quorum

Query multiple replicas:

- Node A -> John
- Node B -> John
- Node C -> Johnny

Choose newest version.

Example:

Suppose:

- `N = 3 replicas`
- Node A
- Node B
- Node C

and:

- `R = 2`

(Read quorum = 2)

Then a read request must get responses from at least 2 replicas before returning.

#### Example 1: All replicas agree

- A -> John
- B -> John
- C -> John

Read from any 2:

- A + B

Return:

`John`

#### Example 2: One replica is stale

A write updated the value to "Johnny" but Node C hasn't received it yet.

- A -> Johnny
- B -> Johnny
- C -> John

Read quorum:

- A + B

or

- A + C

You can determine that "Johnny" is the newest value using:

- timestamps
- version numbers
- vector clocks (more advanced)

and return:

`Johnny`

### Why does R + W > N matter?

Suppose:

- `N = 3`
- `W = 2`
- `R = 2`

A successful write must reach at least 2 replicas.

Example:

Write Johnny

- A ✓
- B ✓
- C not updated yet

Later a read must contact at least 2 replicas.

Possible reads:

- A + B
- A + C
- B + C

Notice every pair includes either A or B, which have the latest write.

So the read will see the newest value.

That's the quorum guarantee.

Example:

- N = 3 replicas
- R = 2
- W = 2

Then:

`R + W = 4`

`4 > 3` ✓

This tells us:

- Every successful read quorum will overlap with every successful write quorum by at least one replica.
- The read quorum and write quorum must overlap by at least one replica.
- So the read operation is guaranteed to contact at least one replica that saw the latest successful write.
- If:

`R + W < N`

- then a read quorum and write quorum can be completely disjoint, meaning the read may never contact a replica that received the latest write.
---

### Why overlap matters

Suppose:

Replicas:
- A
- B
- C

A write succeeds with W=2:

- A ✓
- B ✓

Now a read needs R=2.

Possible read sets:

- A+B
- A+C
- B+C

Notice:

Every read set contains either A or B.

And A/B have the latest write.

So the read will always see the latest data.

---

### Interview Diagram

Write:

```text
Client
  ↓
Coordinator
  ↓
Replica A
Replica B
Replica C
  ↓
Wait for W replicas
  ↓
Success
```

Read:

```text
Client
  ↓
Coordinator
  ↓
Replica A
Replica B
Replica C
  ↓
Collect R responses
  ↓
Return latest version
```

### What if a replica is down?

Answer:

- Write to available replicas
- Recover later
- Hinted handoff / repair

### What if replicas have different values?

Answer:

- Version numbers
- Vector clocks
- Last-write-wins
- Read repair

### How do you balance consistency and availability?

Answer:

- Tune R and W

- `R=1,W=1` → high availability
- `R=2,W=2` → stronger consistency

#### Case 1: R=1, W=1 (High Availability)

Write

To succeed, the write only needs 1 replica.

Write "Johnny"

- A ✓
- B down
- C down

Write succeeds.

Read

Need only 1 replica.

Read from A

Works.

Even if:

- B is down
- C is down

the system is still operational.

That's why availability is high.

##### Downside

Suppose:

- A = Johnny
- B = John
- C = John

A read might hit B:

Read from B
→ John

which is stale.

So consistency is weak.

#### Case 2: R=2, W=2 (Stronger Consistency)

Write

Need 2 acknowledgements.

Write Johnny

- A ✓
- B ✓
- C pending

Success.

Read

Need responses from 2 replicas.

Read A + C

- A -> Johnny
- C -> John

Since A has the newer version, the system returns:

`Johnny`

Much less chance of stale reads.

### Why consistency improves

Because:

- `R + W > N`
- `2 + 2 > 3`

A read quorum must overlap with a write quorum.

So at least one node in the read set has seen the latest write.

## Strong Consistency

Suppose:

- `RF = 3`
- Node A
- Node B
- Node C

Initially:

- A = John
- B = John
- C = John

When a client writes:

`PUT(user:123, "Johnny")`

The system does not acknowledge success immediately.

It first makes sure the required replicas have the new value.

- A = Johnny ✓
- B = Johnny ✓
- C = Johnny ✓

Then:

Return SUCCESS

### What happens on a read?

Immediately after write succeeds:

`GET(user:123)`

Any replica you read from returns:

`Johnny`

Always.

No stale data.

#### Timeline

- Write Johnny
  ↓
- Write committed
  ↓
- Read
  ↓
- Johnny

**Guarantee:**

Once a write completes, every future read sees that write.

### Why is it slower?

Suppose Node C is slow.

- A ✓
- B ✓
- C .....

The write may have to wait.

So:

- Higher latency

### Why lower availability?

Suppose:

- B down
- C down

System may refuse writes because it cannot guarantee consistency.

## Eventual Consistency

Now suppose:

`PUT(user:123, "Johnny")`

The system may do:

- A = Johnny ✓

and immediately return:

`SUCCESS`

before B and C are updated.

State after write:

- A = Johnny
- B = John
- C = John

Different replicas have different values.

### Read immediately

If read hits:

- A

you get:

`Johnny`

If read hits:

- B

you get:

`John`

(stale data)

### Later

Background replication happens:

- A = Johnny
- B = Johnny
- C = Johnny

Eventually all replicas agree.

Hence the name:

`Eventual Consistency`

## What is a Network Partition?

Suppose we have:

`Node A <----X----> Node B`

The network link breaks.

Now:

- Node A
- Node B

Both nodes are alive, but they cannot talk to each other.

This is called a partition.

## Consistency (C)

Consistency means:

Every read gets the latest write.

Example:

Write:

`user = Johnny`

After the write succeeds:

- Read from A -> Johnny
- Read from B -> Johnny
- Read from C -> Johnny

Everyone sees the same value.

## Availability (A)

Availability means:

Every request receives a response.

Not necessarily the latest response.

Example:

`GET(user)`

The system always returns something:

- Johnny
- or John

but it doesn't fail.

## Partition Tolerance (P)

Partition tolerance means:

The system continues operating even when network partitions occur.

Example:

`Node A  <---X--->  Node B`

The network is broken, but the system still tries to serve requests.

## The Key Idea

Many people think:

Choose any 2 of C, A, P

That's the simplified textbook version.

In real systems:

- P is not optional
- Networks can fail.

If you're building a distributed system, you must tolerate partitions.

So during a partition, you choose between:

- Consistency
- Availability

### Example

Suppose:

- A = Johnny
- B = John

and:

`A <---X---> B`

cannot communicate.

### Choose Consistency (CP)

User reads from B.

B is unsure whether it has the latest value.

So B says:

- ERROR
- Cannot serve request

No stale data.

Consistency preserved.

Availability lost.

### Choose Availability (AP)

User reads from B.

B returns:

- John

even though it might be stale.

Availability preserved.

Consistency lost.

## Handling Node Failure

Suppose we have:

- Node A
- Node B
- Node C
- Node D

### Heartbeats

Each node periodically sends a small message:

`"I'm alive"`

A node does not send heartbeats to all nodes. That would be expensive.

Suppose you have:

- 1000 nodes

If every node sends heartbeats to every other node:

- `1000 × 999`

messages every interval.

That's huge.

Instead, each node communicates with only a few peers.

Example:

- A
- B
- C
- D
- E

Round 1

- A -> B : alive
- C -> D : alive
- E -> A : alive

Round 2

- A -> C : alive
- B -> E : alive
- D -> A : alive

Over time, everyone learns about everyone else.

For example every second:

- A -> B : alive
- B -> C : alive
- C -> D : alive
- D -> A : alive

### Normal State

Time 1:

- B -> A : alive

Time 2:

- B -> A : alive

Time 3:

- B -> A : alive

Everything is healthy.

### Node Failure

Suppose Node B crashes:

`Node B ❌`

Now:

- Time 4: (no heartbeat)
- Time 5: (no heartbeat)
- Time 6: (no heartbeat)

After some timeout:

- Node B marked DEAD

### Why Not Immediately?

Because networks are imperfect.

A heartbeat may be delayed.

Heartbeat expected at 10:00

Arrives at 10:00:02

That doesn't mean the node died.

So systems usually wait:

- Miss 3 heartbeats
- or Wait 10 seconds

before declaring failure.

## Gossip Protocol

Now the next question:

How do all nodes learn B is dead?

Having every node talk to every other node is expensive.

Instead use gossip.

### Example

Initially:

- A
- B
- C
- D

A detects:

- B is dead

Round 1

A randomly tells C:

- A -> C
- "B is dead"

Round 2

C tells D:

- C -> D
- "B is dead"

Round 3

D tells A or others:

- D -> A
- "I also know B is dead"

Soon everyone knows.

## Conflict Resolution

The problem is:

- `RF = 3`
- A = John
- B = John
- C = John

Now two users update the same key at almost the same time.

### Example Conflict

User 1:

`PUT(user:123, "Johnny")`

User 2:

`PUT(user:123, "John Smith")`

Because replicas are distributed, you may end up with:

- Node A -> Johnny
- Node B -> John Smith
- Node C -> Johnny

Now:

Which value is correct?

This is called a write conflict.

## Last Write Wins (LWW)

Simplest approach:

Store a timestamp with every write.

Example:

- Johnny      @ 10:00:05
- John Smith  @ 10:00:07

Since:

`10:00:07 > 10:00:05`

Keep:

- John Smith

and discard:

- Johnny

### Problem with LWW

Suppose:

- User 1 updates address.
  - Address = NYC
- User 2 updates phone.
  - Phone = 1234

If User 2's timestamp is later:

- Phone = 1234

wins.

User 1's update may be lost.

This is called a lost update.

## Vector Clocks

Instead of timestamps, track who updated what.

Suppose:

- A:3
- B:2
- C:5

means:

- Node A has seen 3 updates
- Node B has seen 2 updates
- Node C has seen 5 updates

Think of it as a version history.

### Example

Initial value:

- user = John
- VC = {A:1}

Now the network is partitioned.

User 1 updates through Node A:

- user = Johnny
- VC = {A:2}

User 2 updates through Node B:

- user = John Smith
- VC = {A:1, B:1}

Later the partition heals.

System sees:

- Johnny      {A:2}
- John Smith  {A:1, B:1}

Neither vector clock dominates the other.

So the system concludes:

- Concurrent updates detected

but cannot say:

- Johnny is correct
- or John Smith is correct

because both are valid updates.

Vector clock comparison tells us:

- These updates happened independently

instead of:

- One is newer

### What happens then?

System detects:

- Conflict!

and can:

#### Option 1

Return both versions:

- Johnny
- John Smith

Application decides.

#### Option 2

Merge them.

For example, shopping carts:

- Cart A:
  - Apple
- Cart B:
  - Banana

Merge:

- Apple
- Banana

### Why Vector Clocks Exist

LWW says:

- Pick one winner
- Throw away others

Vector clocks say:

- Wait

These writes were concurrent.

Don't lose data.

Vector clocks do not tell you which value is correct.

They tell you:

`"These two writes happened independently (concurrently), so I cannot determine a winner automatically."`

---

## Writes using an LSM Tree (Log Structured Merge Tree).

### Why not write directly to disk?

Suppose every write updates a disk file:

`PUT(user:123, John)`

Disk update:

- Find location
- Modify file
- Update index

Random disk writes are expensive.

So LSM trees convert:

- Many random writes

into

- Sequential writes

which are much faster.

## Write Path

### Step 1: WAL (Write Ahead Log)

User sends:

`PUT(user:123, John)`

First write:

- WAL

Example:

- PUT user:123 John
- PUT user:456 Alice
- PUT user:789 Bob

Think of WAL as:

- Append-only journal

Why?

If the server crashes:

- Power Off ❌

we can replay the WAL and recover data.

This provides:

- Durability

### Step 2: MemTable

At the same time:

`PUT(user:123, John)`

is inserted into:

- MemTable

which lives in RAM.

Example:

- user:123 -> John
- user:456 -> Alice
- user:789 -> Bob

Usually implemented as:

- Skip List
- Balanced Tree

so keys remain sorted.

### Step 3: Flush to SSTable

When MemTable becomes large:

- 100 MB

(or some threshold)

it is flushed to disk.

Create:

- SSTable #1
  - user:123 -> John
  - user:456 -> Alice
  - user:789 -> Bob

An SSTable is:

- Immutable

Meaning:

- Never modified

An SSTable is not the hard disk itself.

An SSTable is a file stored on disk.

Think:

- Hard Disk
  - SSTable_1.db
  - SSTable_2.db
  - SSTable_3.db
  - SSTable_4.db

after creation.

### Why is this fast?

Instead of:

- Random disk updates

we do:

- Sequential append to WAL

and later:

- Sequential flush to SSTable

Disks and SSDs love sequential writes.

Hence:

- Very high write throughput

## Read Problem

Now imagine:

- MemTable
- SSTable 1
- SSTable 2
- SSTable 3
- SSTable 4
- SSTable 5

Looking for:

`GET(user:999)`

Need to check:

- MemTable
- many SSTables

Potentially expensive.

## Bloom Filter

A Bloom Filter is a space-efficient **probabilistic data structure** used to quickly answer:

`"Is this key definitely not present, or might it be present?"`
- It's heavily used in distributed key-value stores like Apache Cassandra, RocksDB, and LevelDB to avoid unnecessary disk reads.

**Question:**

**Does user:999 exist in SSTable 4?**

Possible answers:
```
- Definitely Not
  - No
```
100% guaranteed.

Skip reading SSTable 4.
```
- Maybe Yes
  - Maybe
```
Need to check SSTable 4.

### Important Property

Bloom filters can have:
```
- False Positives
```
**Example:**

Bloom Filter says:

`"Maybe present"`

but key is not actually there.

That's okay.

We just do an extra disk lookup.

Bloom filters never have:
```
- False Negatives
```
They will never say:

`Definitely not present`

when the key actually exists.

### Example

Without Bloom Filters:

`GET(user:999)`

- Check SSTable1
- Check SSTable2
- Check SSTable3
- Check SSTable4
- Check SSTable5

Lots of disk IO.

With Bloom Filters:

- BF1 -> Not Present
- BF2 -> Not Present
- BF3 -> Not Present
- BF4 -> Not Present
- BF5 -> Maybe

Only open:

- SSTable5

Huge performance gain.

---

## Neat Flow of Write and Read on Nodes

In most distributed key-value store discussions, a node is essentially a **server running the database software**.

Think of it like this:

```text
Cluster
│
├── Node A (Database Instance)
├── Node B (Database Instance)
├── Node C (Database Instance)
└── Node D (Database Instance)
```

Each node has:

- CPU
- Memory
- Disk
- Database Software

For example, in a Cassandra cluster:

- Node A = Cassandra process + local disk
- Node B = Cassandra process + local disk
- Node C = Cassandra process + local disk

### What is database ?
A database is a software program.

Examples:
```
MySQL
PostgreSQL
MongoDB
Apache Cassandra
```
Just like:
```
Chrome = software
VS Code = software
MySQL = software
```
- The database runs in memory (RAM + CPU) as a process.
- Its data is stored on disk.
- The database software's job is to read and write data to disk (and also manage memory, indexes, caching, transactions, etc.).
- The data is stored on disk, and the database software reads it from disk and presents it to applications/users.

### What happens during a Write?

Suppose:

`PUT("user123", "John")`

#### Step 1: Find the responsible node

Using consistent hashing:
```
`hash(user123)`
    ↓
`Node B`
```
Node B becomes the primary/coordinator.

#### Step 2: Replication

Assume:

- `RF = 3`

Then data is stored on:

- Node B (Primary)
- Node C (Replica)
- Node D (Replica)

#### On each node

The node performs its local database write:

```text
Write Ahead Log (WAL)
        ↓
MemTable
        ↓
SSTable
```

So yes, every replica node executes its own write path locally.

#### Write Flow Diagram

```text
Client
   ↓
Coordinator (B)
   ↓
B writes locally
C writes locally
D writes locally
```

### What happens during a Read?

Suppose:

`GET("user123")`

Coordinator determines replicas:

- B
- C
- D

Assume:

- `R = 2`

It queries two replicas.

- Read from B
- Read from C

Each node executes its own local read:

- Check MemTable
- Check Bloom Filter
- Check SSTables
- Return value

Coordinator compares versions and returns the newest value.




## Interview-Level Complete Design

```text
                    Clients
                        |
                  Load Balancer
                        |
                Coordinator Layer
                        |
             Consistent Hash Ring
                        |
     ------------------------------------------------
     |               |               |             |
     v               v               v             v

   Node A          Node B         Node C        Node D

     |               |               |             |
     |<----Replication Factor = 3---->|
     |
     v

 WAL -> MemTable -> SSTables
           |
           v
      Bloom Filters
```
