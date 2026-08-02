# Hot Partition (or Hot Shard)

A hot partition is a database or cache partition that receives a disproportionately large amount of traffic compared to the others.

As a result:

- CPU usage increases
- Memory usage increases
- Network bandwidth gets saturated
- Latency increases

That partition may crash while the others remain mostly idle.

## Simple Example

Suppose your Tweets table is partitioned by:

```text
User ID % 4
```

### Partitions

- Partition 1: User 1, User 5, User 9
- Partition 2: User 2, User 6, User 10
- Partition 3: User 3, User 7, User 11
- Partition 4: User 4, User 8, User 12

Normally, traffic is balanced.

Now imagine that User 8 is Cristiano Ronaldo.

Every second, 5 million users read Ronaldo's tweets.

Since User 8 belongs to Partition 4, the traffic becomes heavily skewed:

```text
Partition 1   5%
Partition 2   8%
Partition 3   7%
Partition 4   80%
```

```text
DB Cluster
+---------+
| Shard 1 | 5%
+---------+

+---------+
| Shard 2 | 8%
+---------+

+---------+
| Shard 3 | 7%
+---------+

+---------+
| Shard 4 | 80% ??
+---------+
```

Shard 4 becomes a hot partition.

## Why Is This Bad?

Imagine each server handles 1000 requests per second.

Now Shard 4 receives 100,000 requests per second.

### Result

- CPU = 100%
- Memory = full
- Network = saturated
- Latency = high
- Timeouts occur

Eventually, the shard crashes even though Shard 1, Shard 2, and Shard 3 are mostly idle.

## Hot Partition in Cache

The same problem can happen with Redis.

Suppose a Redis cluster has four nodes:

- Node 1
- Node 2
- Node 3
- Node 4

Everyone requests Ronaldo's latest tweet. If the cache key `tweet:ronaldo` is stored only on Node 2, then Node 2 receives millions of reads while the other Redis nodes do nothing.

This is called a hot key, which creates a hot partition.

## Solutions

### 1. Caching (Most Common)

Instead of hitting the database directly:

```text
Users -> Redis -> Database
```

Most requests are served from Redis.

### 2. Replication

Instead of keeping only one copy of a shard, create multiple read replicas.

```text
Users -> Read Replica 1
Users -> Read Replica 2
Users -> Read Replica 3
```

Now requests are distributed across replicas.

### 3. Replicate Hot Cache Keys

Instaed of just having one key in Redis, better approach is to `keep multiple cache keys in Redis`, but there is an important nuance.

#### Normal Cache

Suppose Ronaldo's latest tweet is cached as:

```text
tweet:ronaldo -> "Hello World"
```

It is stored on Redis Node 2.

```text
Redis Cluster
+--------+   +--------+
| Node1  |   | Node2  |
+--------+   +--------+
                 |
         tweet:ronaldo
                 |
             Hello World
```

Now 10 million users request:

```text
GET tweet:ronaldo
```

Every request goes to Node 2.

```text
Users
  |
10 Million GET requests
  |
Node 2 ??
```

Node 2 becomes the bottleneck.

#### Solution 1: Replicate the Cache Entry

Instead of one key `tweet:ronaldo`, create several identical keys:

- `tweet:ronaldo:1`
- `tweet:ronaldo:2`
- `tweet:ronaldo:3`

Each contains exactly the same data.

```text
Redis Node 1 -> tweet:ronaldo:1 -> Hello World
Redis Node 2 -> tweet:ronaldo:2 -> Hello World
Redis Node 3 -> tweet:ronaldo:3 -> Hello World
```

When users request the tweet, the app server randomly chooses one of the keys.

```text
Traffic becomes:
3 Million -> Node 1
3 Million -> Node 2
4 Million -> Node 3
```

instead of:

```text
10 Million -> Node 2
```

This distributes the load.

**But doesn't this waste memory?**

- Yes. Instead of storing one copy, you store three copies. Memory usage increases.

- However, memory is often cheaper than having your hottest cache node overloaded and causing high latency or outages.

#### Solution 2: Redis Replication (More Common)

In production, it is more common to rely on Redis replication.

```text
Primary
  |
  +-- Replica 1
  +-- Replica 2
  +-- Replica 3
```

The application load-balances read requests across replicas.

```text
User -> Load Balancer -> Replica 1 / Replica 2 / Replica 3
```

This avoids manually creating keys like:

- `tweet:ronaldo:1`
- `tweet:ronaldo:2`

### 4. Better Partitioning

Avoid partitioning on fields that create uneven traffic.

For example:

```text
Bad: Shard = UserID % N
```

If one user is extremely popular, that shard gets overloaded.

Sometimes systems partition by other attributes or use more advanced strategies to balance load.

### 5. CDN (For Images and Videos)

For Instagram Reels or YouTube videos:

```text
User -> CDN -> Origin Server
```

Millions of users fetch the content from geographically distributed CDN edge servers instead of one backend.
