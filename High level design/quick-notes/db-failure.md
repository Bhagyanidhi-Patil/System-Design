# Database Failure Scenarios

A database can fail in many different ways. Instead of treating it as one problem, think of it as several failure modes.

## System Architecture

```text
                    Client
                       |
                 Load Balancer
                       |
               Application Servers
                       |
               ---------------------
               |                   |
          Primary Database    Read Replica(s)
```

### Normal Flow

```text
Client
   |
Application
   |
Primary Database (Writes)

Read Replica(s) (Reads)
```

---

# Scenario 1: Primary Database Crash ⭐⭐⭐ (Most Important)

## Situation

Suppose your primary database crashes.

```text
Application
     |
Primary Database ❌
```

Since all write operations go to the primary database:

* INSERT fails
* UPDATE fails
* DELETE fails

Users can no longer modify data.

---

## Impact

Example: Instagram

Users cannot:

* Like posts
* Comment
* Upload photos
* Follow users

Even login may fail if authentication requires updating user sessions.

---

## Solution 1: Primary-Replica Replication

Instead of deploying only one database:

```text
              Primary
             /       \
      Replica 1   Replica 2
```

* Primary handles all write operations.
* Replicas continuously synchronize data from the primary.

---

## Automatic Failover

If the primary crashes:

```text
Before

Primary
   |
Replica

After

Primary ❌

Replica → New Primary
```

The application reconnects to the promoted replica.

Typical downtime is only a few seconds.

---

## Common Technologies

Examples:

* MySQL Group Replication
* PostgreSQL Streaming Replication
* MongoDB Replica Sets

---

# Scenario 2: Read Replica Failure

Suppose:

```text
Primary

Replica 1 ❌

Replica 2
```

One replica crashes.

### Impact

* Writes continue normally.
* Reads are redirected to another healthy replica.
* Overall impact is minimal.

---

## Solution

Deploy multiple read replicas.

```text
Primary

Replica 1

Replica 2

Replica 3
```

A load balancer distributes read traffic among healthy replicas.

---

# Scenario 3: Entire Database Cluster Failure

Suppose:

```text
Primary ❌

Replica 1 ❌

Replica 2 ❌
```

The entire database cluster becomes unavailable.

### Possible Causes

* Data center outage
* Cloud provider outage
* Power failure
* Network failure

---

## Impact

Everything stops.

* Reads fail
* Writes fail

The application becomes unavailable.

---

## Solution 1: Multi-Region Deployment

```text
Region A

Primary Database

↓

Replication

↓

Region B

Standby Database
```

If Region A fails,

traffic is redirected to Region B.

---

## Solution 2: Disaster Recovery

Maintain regular backups.

Examples:

* Daily full snapshots
* Hourly incremental backups
* Write-Ahead Logs (WAL)

The database can be restored after a catastrophic failure.


## Database Primary and Replicas Across AZs

You have **one primary DB** and **one or more replicas**, and they are placed across different AZs.

```text
                     Region
            ┌──────────┼──────────┐
            ↓          ↓          ↓
          AZ-1       AZ-2       AZ-3
            │          │          │
        Primary DB   Replica    Replica
            │          ↑          ↑
            └──────────┴──────────┘
                 Replication
```

So **NOT**:

```text
❌ AZ-1 → Primary + Replica
   AZ-2 → Primary + Replica
   AZ-3 → Primary + Replica
```

You don't normally have three independent primaries just because you have three AZs.

---

## Why Put Them in Different AZs?

Suppose:

```text
AZ-1 → Primary DB
AZ-2 → Replica DB
AZ-3 → Replica DB
```

Normally:

```text
                     Application
                          |
                          ↓
                     Primary DB
                          |
                 ┌────────┴────────┐
                 ↓                 ↓
             Replica 1          Replica 2
              (AZ-2)             (AZ-3)
```

If **AZ-1 fails**:

```text
AZ-1 → ❌ Primary DB
AZ-2 → ✅ Replica
AZ-3 → ✅ Replica
```

One replica can be promoted to become the new primary:

```text
AZ-2 → NEW PRIMARY
AZ-3 → Replica
```

This gives you **high availability**.

---

## Important Distinction

There are different database architectures.

### Traditional Primary-Replica

```text
AZ-1                AZ-2                AZ-3

Primary  ───────→  Replica 1
    │
    └────────────→  Replica 2
```

**One primary handles writes.**

**Replicas maintain copies and can handle reads.**

---

### Multi-Primary / Distributed Databases

Some database technologies support multiple nodes that can accept writes:

```text
AZ-1          AZ-2          AZ-3
  │             │             │
  DB  ←──────→  DB  ←──────→  DB
  ↑             ↑             ↑
  └──── All can potentially accept writes ────┘
```

That's a **different architecture** and has additional consistency/conflict considerations.

---
## Database Cluster vs Availability Zone

**A database cluster does NOT necessarily mean all its nodes are in the same AZ.**

A **cluster** is a logical grouping of database instances/nodes that work together. Those nodes can be distributed across multiple AZs.

For example:

```text
                     DB CLUSTER
                         │
            ┌────────────┼────────────┐
            ↓            ↓            ↓
          AZ-1         AZ-2         AZ-3
            │            │            │
        Primary DB    Replica 1    Replica 2
```

Here, all three databases belong to **one cluster**, but they are in **different AZs**.

---

## But a Cluster CAN Also Be Within One AZ

For example:

```text
AZ-1
│
└── DB Cluster
     ├── Primary
     └── Replica
```

Whether nodes are spread across AZs depends on the **database technology and how it is configured**.

`Usually, if one database node goes down, a replica/node within the same cluster takes over. It is not necessarily another cluster taking over.`

You **can** have multiple clusters, for example:

```text
Cluster A
├── Primary
├── Replica
└── Replica

Cluster B
├── Primary
├── Replica
└── Replica
```

You might configure **Cluster B as a standby/disaster-recovery cluster**.

If the entire Cluster A becomes unavailable:

```text
Cluster A → ❌ DOWN

        ↓

Cluster B → ✅ Takes over
```

But this is generally a **disaster-recovery setup**, not the normal meaning of database replication.


---

# Scenario 4: Database Overload

The database is healthy but receives more requests than it can handle.

### Example

```text
Database Capacity

20,000 requests/sec

Incoming Traffic

100,000 requests/sec
```

CPU utilization reaches 100%.

Queries become slow.

---

## Solutions

### 1. Read Replicas

Distribute read traffic.

```text
               Primary
           /      |      \
     Replica1 Replica2 Replica3
```

---

### 2. Cache

Instead of:

```text
Every Request

↓

Database
```

Use:

```text
Redis

↓

Database
```

The cache significantly reduces database load.

---

### 3. Rate Limiting

Protect the database.

```text
100,000 Requests

↓

Allow

20,000

↓

Reject / Queue

80,000
```

---

### 4. Query Optimization

Slow queries consume excessive resources.

Bad query:

```sql
SELECT * FROM Orders;
```

Optimized query:

```sql
SELECT order_id
FROM Orders
WHERE user_id = 101;
```

Best practices:

* Create indexes
* Use pagination
* Optimize joins

---

# Scenario 5: Slow Queries

The database is operational but responds slowly.

### Common Causes

* Missing indexes
* Full table scans
* Expensive joins

---

## Solution

Use indexes.

Without an index:

```text
1 Million Rows

↓

Scan Every Row
```

With an index:

```text
B+ Tree

↓

Locate Row

O(log n)
```

---

# Scenario 6: Deadlocks

Two transactions wait indefinitely for each other.

### Example

Transaction A

```text
Lock User

↓

Needs Order
```

Transaction B

```text
Lock Order

↓

Needs User
```

Both transactions remain blocked.

---

## Solution

The database detects the deadlock.

One transaction is rolled back automatically.

The application retries the failed transaction.

---

# Scenario 7: Network Partition

The application cannot reach the database.

The database itself is healthy.

```text
Application

↓

Network ❌

↓

Database
```

---

## Solution

Use a Circuit Breaker.

Instead of retrying indefinitely:

* Configure a timeout (for example, 200 ms).
* Return an error or fallback response.
* Retry later.

---

# Scenario 8: Disk Failure

The storage device containing the database crashes.

Database files become inaccessible.

---

## Solution

* RAID
* Cloud block storage
* Database replication
* Regular backups

---

# Scenario 9: Data Corruption

Example:

A software bug incorrectly updates every user's account balance.

The database is running, but the stored data is incorrect.

---

## Solution

Use Point-in-Time Recovery.

```text
Backup

↓

Replay Transaction Logs

↓

Recover Database
```

---

# Scenario 10: Connection Pool Exhaustion

Suppose:

* Application opens 1000 database connections.
* Database supports only 200.

Remaining requests wait for an available connection.

---

## Solution

Use Connection Pooling.

```text
Application

↓

Connection Pool (50)

↓

Database
```

Connections are reused instead of creating a new connection for every request.

---

# Scenario 11: Storage Full

Disk utilization reaches 100%.

The database can no longer perform write operations.

---

## Solution

* Monitoring
* Auto Scaling
* Alerts
* Archive old data
* Increase storage capacity

---

# Scenario 12: Split Brain

A network partition causes two databases to believe they are the primary.

```text
Primary A

Primary B
```

Both accept writes independently.

Data becomes inconsistent.

---

## Solution

Use leader election and quorum-based consensus.

Common algorithms:

* Raft
* Paxos

These ensure that only one database can act as the primary at any given time.
