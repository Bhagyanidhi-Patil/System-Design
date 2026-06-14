# How would you handle hot partitions?
## What is a Hot Partition?

- A hot partition happens when one partition/shard receives a disproportionately large amount of traffic (reads, writes, or both) compared to other partitions. This creates a bottleneck because the overloaded partition reaches its CPU, memory, network, or storage limits while other partitions remain underutilized.

- Think of it like a supermarket with 10 checkout counters where 90% of customers stand in one line. The store has plenty of capacity overall, but that one line becomes painfully slow.

Consider a distributed database:

```
Users Table

Shard 1: User IDs 1–1M      → 5% traffic
Shard 2: User IDs 1M–2M     → 8% traffic
Shard 3: User IDs 2M–3M     → 80% traffic 🔥
Shard 4: User IDs 3M–4M     → 7% traffic
```

Even if the cluster has plenty of total capacity, the overloaded shard becomes the bottleneck.

---

## Why Hot Partitions Are Bad ?

A hot partition can create several serious problems in a distributed system:

### Increased Latency

When requests arrive faster than a partition can process them, a queue starts building up.

```
Incoming Traffic: 10,000 req/sec
Partition Capacity: 2,000 req/sec

Queue grows
↓
Latency increases
```

As the backlog grows, users experience slower response times.

---

### Reduced Throughput

The overall throughput of a distributed system is often limited by its busiest partition.

```
Total Cluster Capacity = 100,000 req/sec

Hottest Shard Capacity = 5,000 req/sec

Effective System Throughput ≈ 5,000 req/sec
```

Even though the cluster has significant unused capacity elsewhere, the hot shard becomes the limiting factor.

---

###  Cascading Failures

A hot partition can trigger a chain reaction across the system:

```
Hot shard overloaded
        ↓
Timeouts increase
        ↓
Retries increase
        ↓
More load generated
        ↓
Shard crashes
        ↓
Traffic shifts elsewhere
        ↓
Additional shards become overloaded
```

This feedback loop can quickly escalate into a large-scale outage if not properly handled.

---

## Common Causes of Hot Partitions


### 1. Poor Partition Key (Uneven Distribution)

### Problem

The partition key should distribute both data and traffic evenly. If one key value is significantly more common than others, a single partition can become overloaded while the rest remain underutilized.

### Example

Suppose a database partitions data using:

```text
partition_key = country
```

Traffic distribution:

```text
USA      -> 90% of requests
India    -> 5%
Others   -> 5%
```

Let's assume:

```text
USA      -> Shard 1
India    -> Shard 2
Others   -> Shard 3
```

Result:

```text
Shard 1 -> 90% traffic 🔥
Shard 2 -> 5%
Shard 3 -> 5%
```

### Why It Happens

Even if data is partitioned correctly, user behavior may not be evenly distributed.

For example:

* Most customers are from the USA.
* Most searches target USA records.
* Most writes belong to USA users.

### Impact

```text
Shard 1 latency increases
Shard 1 CPU reaches 100%
Requests start timing out
Other shards remain mostly idle
```

### Better Approach

Use a higher-cardinality key:

```text
partition_key = user_id
```

instead of:

```text
partition_key = country
```

Since there are millions of unique user IDs, requests are spread much more evenly across partitions.

---

### 2. Sequential Keys (Traffic Concentrates on New Data)

### Problem

Keys are generated in increasing order:

```text
1001
1002
1003
1004
1005
...
```

In many systems, recently created records receive the majority of traffic.

### Example

Suppose data is partitioned by order ID ranges:

```text
Shard 1: 1 - 1000
Shard 2: 1001 - 2000
Shard 3: 2001 - 3000
```

Current orders:

```text
2998
2999
3000
```

Almost all new writes go to:

```text
Shard 3 🔥
```

Traffic distribution:

```text
Shard 1 -> 5%
Shard 2 -> 10%
Shard 3 -> 85%
```

### Why It Happens

Users typically interact with recent orders:

```text
Create Order
Update Order
Track Order
Pay for Order
```

These operations repeatedly hit the newest partition.

### Better Approach

Use hashing:

```text
shard = hash(order_id) % N
```

Example:

```text
Order 2998 -> Shard 2
Order 2999 -> Shard 1
Order 3000 -> Shard 4
```

Now traffic is distributed across multiple shards instead of concentrating on the newest one.

---

### 3. Celebrity Problem (Skewed Access Pattern)

### Problem

A single record becomes extremely popular.

Even if data is perfectly distributed, traffic may not be.

### Example

Consider a social media platform:

```text
User A          -> 100 followers
User B          -> 200 followers
User C          -> 300 followers
Taylor Swift    -> 100,000,000 followers
```

Suppose data is partitioned using:

```text
partition_key = user_id
```

Taylor Swift's profile happens to be stored on:

```text
Shard 7
```

Every time followers:

* Open her profile
* Read her posts
* View her timeline
* Like her content

the requests hit:

```text
Shard 7 🔥🔥🔥
```

### Visualization

```text
Shard 1 -> 100 req/sec
Shard 2 -> 80 req/sec
Shard 3 -> 120 req/sec
Shard 7 -> 50,000 req/sec 🔥
```

### Why It Happens

Traffic is determined by popularity, not by the amount of stored data.

A single row or user record can generate millions of requests.

### Common Solutions

* Caching (Redis)
* CDN
* Read replicas
* Fan-out strategies
* Materialized views

Example:

```text
Taylor's Posts
       ↓
   Redis Cache
       ↓
Millions of Reads Served
```

Instead of hitting the database for every request, most reads are served from the cache.

---

### 4. Time-Based Keys (Current Time Becomes Hot)

### Problem

Data is partitioned using time-based values.

Example:

```text
partition_key = current_day
```

Partitions:

```text
2026-06-12
2026-06-13
2026-06-14
```

### What Happens

Today's partition receives almost all new traffic:

```text
All new writes
All new events
All new logs
```

which go to:

```text
2026-06-14 🔥
```

Older partitions receive little or no traffic.

### Example

An event logging system:

```text
Event 1 -> 2026-06-14
Event 2 -> 2026-06-14
Event 3 -> 2026-06-14
```

Traffic distribution:

```text
2026-06-14 -> 99% writes 🔥
2026-06-13 -> 1%
2026-06-12 -> 0%
```

### Impact

```text
Current partition overloaded
Write latency increases
Throughput drops
```

### Better Approach

Add a secondary distribution key.

Instead of:

```text
partition_key = day
```

use:

```text
partition_key = day + hash(user_id)
```

Example:

```text
2026-06-14_01
2026-06-14_02
2026-06-14_03
2026-06-14_04
```

Now today's writes are spread across multiple partitions rather than being concentrated in a single one.


---

## Solutions for Hot Partitions

### Solution 1: Better Partitioning Strategy

The most fundamental solution is to choose a partition key that distributes traffic uniformly.

### Bad Example

```text
partition_key = user_country
```

Since some countries generate significantly more traffic than others, certain partitions become overloaded.

### Better Example

```text
partition_key = hash(user_id)
```

Example:

```text
hash(user1) → Shard 3
hash(user2) → Shard 8
hash(user3) → Shard 1
```

Traffic is distributed more evenly across shards.

### Why Hashing Helps

Without hashing:

```text
USA   → Shard 1
India → Shard 2
```

Most requests may end up on a single shard.

With hashing:

```text
hash(USA-user1)
hash(USA-user2)
hash(USA-user3)
```

Users are spread across many partitions, preventing hotspots.

---

### Solution 2: Consistent Hashing

Traditional partitioning often uses:

```text
hash(key) % N
```

The problem is that when a node is added or removed, most keys need to be reassigned.

Consistent hashing places nodes and keys on a hash ring.
```
| Normal Hashing                   | Consistent Hashing                        |
| -------------------------------- | ----------------------------------------- |
| hash(key) % N                    | hash(key) → next clockwise node           |
| Depends on number of servers     | Independent of number of servers          |
| Adding a server remaps most keys | Adding a server remaps only a small range |
| Simple                           | More scalable                             |
```
### Benefits

* Better key distribution
* Minimal data movement during scaling
* Easier node addition and removal
* Reduced re-sharding costs

Commonly used in:

* Apache Cassandra
* Amazon DynamoDB
* Distributed caching systems

---

### Solution 3: Virtual Nodes (VNodes)

Even with consistent hashing, partitions may still be uneven.

Example:

```text
Node A → Huge range
Node B → Small range
```

To improve balance, each physical machine owns multiple virtual nodes.

Example:

```text
Physical Node A
 ├─ vnode1
 ├─ vnode2
 └─ vnode3

Physical Node B
 ├─ vnode4
 ├─ vnode5
 └─ vnode6
```

### Benefits

* Better load balancing
* More uniform data distribution
* Easier re-distribution during scaling
* Faster recovery from failures

Virtual nodes are a common discussion point in system design interviews.

---

### Solution 4: Key Salting (Write Sharding)

Key salting is a popular technique for handling write hotspots.

Suppose:

```text
Product 123
```

receives:

```text
100,000 writes/sec
```

Instead of storing all writes under:

```text
product:123
```

Create multiple logical keys:

```text
product:123:0
product:123:1
product:123:2
product:123:3
```

Choose a shard using:

```text
hash(request) % 4
```

### Before

```text
All Writes
     ↓
Partition 5 🔥
```

### After

```text
25% → Partition 1
25% → Partition 2
25% → Partition 3
25% → Partition 4
```

### Drawback

Reads become more expensive.

To compute the total count:

```text
Read shard 0
Read shard 1
Read shard 2
Read shard 3

Aggregate results
```

This read-versus-write trade-off is frequently discussed in interviews.

---

### Solution 5: Replication for Read Hotspots

When the workload is primarily read-heavy:

```text
1,000,000 reads/sec
100 writes/sec
```

Add read replicas.

```text
            Primary
          /    |    \
   Replica1 Replica2 Replica3
```

Requests are distributed across replicas:

```text
Client
  ↓
Load Balancer
  ↓
Replica Pool
```

### Benefits

* Higher read throughput
* Reduced load on primary database
* Improved availability

Commonly used in:

* MySQL
* PostgreSQL
* MongoDB

---

### Solution 6: Caching

Caching is one of the most effective solutions for celebrity-style hotspots.

### Without Cache

```text
Users
  ↓
Database 🔥
```

### With Cache

```text
Users
  ↓
Cache
  ↓
Database
```

### Example

Traffic:

```text
100,000 requests/sec
```

Cache hit rate:

```text
95%
```

Database traffic becomes:

```text
100,000 × 5%
= 5,000 requests/sec
```

This dramatically reduces database load.

Popular caching systems:

* Redis
* Memcached

---

### Solution 7: CDN for Extreme Read Hotspots

For static content such as:

* Videos
* Images
* Profile pictures

Use a Content Delivery Network (CDN).

```text
User
 ↓
CDN Edge Server
 ↓
Origin Server
```

### Benefits

* Lower latency
* Reduced origin traffic
* Better global performance

Popular CDNs:

* Cloudflare
* Akamai
* Amazon CloudFront

---

### Solution 8: Dynamic Re-Sharding

When a shard becomes too large or receives too much traffic, split it into smaller shards.

### Before

```text
Shard A
500 GB
```

### After

```text
Shard A1
Shard A2
```

Traffic is now distributed:

```text
50%
 ↓
Shard A1

50%
 ↓
Shard A2
```

Example:

```text
Users A-M → Shard A1
Users N-Z → Shard A2
```

This approach is widely used in large-scale distributed databases.

---

### Solution 9: Adaptive Partitioning

Modern databases continuously monitor partition health.

Metrics include:

```text
CPU Usage
Memory Usage
Traffic Volume
Latency
```

If thresholds are exceeded:

```text
CPU > 80%
Latency > Threshold
Traffic > Threshold
```

The system automatically:

* Splits shards
* Adds replicas
* Migrates partitions
* Rebalances traffic

This enables automatic scaling and hotspot mitigation.

---

### Solution 10: Queue-Based Load Smoothing

Useful when traffic arrives in bursts.

### Direct Writes

```text
Client
  ↓
Database
```

A sudden spike can overload the database.

### Queue-Based Architecture

```text
Client
  ↓
Message Queue
  ↓
Workers
  ↓
Database
```

Examples:

* Apache Kafka
* RabbitMQ
* Amazon SQS

### Benefits

* Absorbs traffic spikes
* Smooths write rates
* Protects hot partitions
* Improves system stability

The queue acts as a buffer between incoming traffic and the database.
