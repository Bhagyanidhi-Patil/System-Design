# Cache Failure in HLD

## What is Cache Failure?

Cache failure occurs when the cache layer cannot serve requests correctly due to failures such as:

* Complete cache outage
* Network issues
* Cache node failures
* Cache misses after restart
* Cache inconsistencies
* Memory eviction

When the cache fails, requests usually fall back to the database, which can overwhelm it and significantly increase response times.

---

# System Architecture

```text
                    Client
                       |
                 Load Balancer
                       |
              -------------------
              |                 |
          App Server        App Server
              |                 |
              -------------------
                       |
                 Redis Cluster
                       |
                  Primary Database
```

### Normal Flow

```text
Request
   |
Redis (Cache HIT)
   |
Response
```

### Typical Latency

```text
Redis      : 1–5 ms
Database   : 20–100 ms
```

---

# Scenario 1: Complete Cache Crash (Most Important)

## Redis Failure Scenarios in HLD

There are **two different Redis failure scenarios**.

Understanding the difference is important because the solutions are completely different.

---

## Scenario A: One Redis Node Goes Down (Common)

Suppose your Redis Cluster looks like this:

```text
                 Redis Cluster
        -----------------------------
        |            |              |
     Master 1     Master 2      Master 3
        |            |              |
     Replica 1    Replica 2     Replica 3
```

Each master stores a portion of the data (**sharding**).

For example:

```text
Master 1 → User Data

Master 2 → Product Data

Master 3 → Order Data
```

Now suppose:

```text
Master 2 crashes.
```

Redis Sentinel (or Redis Cluster) automatically promotes Replica 2 to become the new master.

```text
Before

Master 2
    |
Replica 2


After

Master 2 ❌

Replica 2 → New Master
```

### What happens?

* Only the data handled by Master 2 is temporarily unavailable.
* Failover usually completes within a few seconds.
* The rest of the cluster continues serving requests.

This is the failure most people refer to as **"Redis failure."**

### Solution

`Redis Replication + Automatic Failover`(Sentinel/Redis Cluster).

---

## Scenario B: Entire Redis Cluster Goes Down

Now imagine something much worse.

```text
                Redis Cluster

        Master 1 ❌

        Master 2 ❌

        Master 3 ❌

        Replica 1 ❌

        Replica 2 ❌

        Replica 3 ❌
```

Everything is unavailable.

### Possible Reasons

* Entire data center loses power.
* Network partition disconnects the whole Redis cluster.
* Cloud provider outage.
* Someone accidentally deletes the Redis deployment.
* A faulty deployment crashes every Redis instance.

Now there is **no healthy replica available**.

Redis Sentinel cannot help because there is nothing left to promote.

---

### What Happens Now?

The application first tries Redis.

```text
Client
   |
Application
   |
Redis ❌
```

Redis returns:

```text
Connection Refused

or

Timeout
```

The application has only one option:

```text
Client
   |
Application
   |
Database
```

Every request now reaches the database.

### Example

```text
Normal Traffic

500,000 requests/sec
```

Database Capacity

```text
20,000 requests/sec
```

After Redis fails,

```text
500,000 Requests
        |
        V
     Database
```

The database quickly becomes overloaded, leading to:

* High latency
* Request timeouts
* Service degradation
* Possible outage

---

### Can Redis Replication Solve This?

**No.**

Replication only protects against **individual node failures**.

It **cannot** recover an entire Redis cluster if every node is unavailable.

This is an important interview distinction.

---

## How Do Companies Handle an Entire Redis Cluster Failure?

### 1. Multi-Region / Multi-Availability Zone Deployment ⭐⭐⭐

Instead of deploying only one Redis cluster,

```text
           Region A

       Redis Cluster A


               |

       Data Replication


               |

           Region B

       Redis Cluster B
```

- If Region A becomes unavailable,

- traffic is redirected to Region B.

- This is the `standard disaster recovery architecture` used in production systems.

- Cloud providers like `AWS, Azure, and GCP` recommend deploying critical services across multiple Availability Zones (AZs) and, when necessary, across multiple Regions.

---

### 2. Local In-Memory Cache

Each application server maintains a small local cache.

```text
Application

↓

Local Cache

↓

Redis

↓

Database
```

If Redis becomes unavailable,

```text
Application

↓

Local Cache

↓

Response
```

Only frequently accessed (hot) data is available, but this significantly reduces the load on the database.

Examples:

* Caffeine Cache
* Guava Cache
* Spring Cache

---

### 3. Graceful Degradation

Instead of failing the entire application, temporarily disable non-essential features.

### Example (Instagram)

Continue serving:

* Feed
* Messages
* Profile

Temporarily disable:

* Trending Posts
* Recommendations
* Suggested Users

Users can still use the application's core functionality.

---

### 4. Circuit Breaker

Without a Circuit Breaker:

```text
Every Request

↓

Wait 200 ms

↓

Redis Timeout

↓

Database
```

Every request wastes time waiting for Redis.

With a Circuit Breaker:

```text
Redis Marked DOWN

↓

Skip Redis

↓

Database
```

The application immediately bypasses Redis and periodically checks whether Redis has recovered.

This significantly reduces latency.

---

### 5. Rate Limiting

Don't allow unlimited requests to reach the database.

```text
500,000 Requests

↓

Allow

20,000

↓

Reject / Queue

480,000
```

Rate limiting protects the database from overload.

---

### 6. Database Read Replicas

Instead of relying on a single database,

```text
           Primary Database
          /                \
 Read Replica 1      Read Replica 2
```

Read traffic can be distributed among replicas, reducing pressure on the primary database during Redis outages.

---

### 7. Disaster Recovery (Persistent Backup)

Redis supports persistent storage using:

* **RDB (Redis Database Snapshots)**
* **AOF (Append Only File)**

If the entire Redis cluster is lost, a new cluster can be restored from the latest backup.

Although this does not eliminate downtime, it minimizes data loss and speeds up recovery.

---

### Interview Takeaway

| Scenario                        | Does Replication Help?         | Solution                                                                                                                     |
| ------------------------------- | ------------------------------ | ---------------------------------------------------------------------------------------------------------------------------- |
| **One Redis Node Fails**        | ✅ Yes                          | Replica Promotion using Sentinel or Redis Cluster                                                                            |
| **One Availability Zone Fails** | ✅ Yes (if deployed across AZs) | Cross-AZ Failover                                                                                                            |
| **Entire Redis Cluster Fails**  | ❌ No                           | Multi-Region Deployment, Local Cache, Circuit Breaker, Graceful Degradation, Rate Limiting, Read Replicas, Disaster Recovery |



---

# Scenario 2: Cache Stampede

> **This is NOT a cache crash.**

Redis is healthy.

Only one cache key expires.

Example:

```text
product:100
```

Now,

```text
10,000 Users
      |
Same Product
```

Without protection,

```text
Request 1 -> Database

Request 2 -> Database

Request 3 -> Database

...

10,000 Database Queries
```

The database becomes overloaded.

---

## Solutions

### Distributed Lock

```text
Cache Miss
    |
Acquire Lock
```

Only one request reaches the database.

```text
Database
    |
Update Cache
    |
Release Lock
```

Remaining requests simply read from the refreshed cache.

Only one database query is executed.

---

### Request Coalescing

Instead of

```text
1000 Database Queries
```

combine them into

```text
1000 Requests
      |
 One Database Query
      |
 Shared Response
```

---

### Stale Cache

Instead of deleting expired data immediately,

```text
Old Cache
     |
User Response
     |
Background Refresh
```

Users receive slightly stale data while the cache refreshes asynchronously.

This approach is commonly used for:

* Product catalogs
* News feeds

---

# Scenario 3: Cache Avalanche

Different from Cache Stampede.

Suppose

```text
1 Million Keys

TTL = 1 Hour
```

After exactly one hour,

```text
Millions of Cache Misses
          |
      Database
```

The database experiences a sudden spike in traffic.

---

## Solutions

Randomize cache expiration.

Instead of

```text
1 Hour
```

Use

```text
58 Minutes

61 Minutes

63 Minutes

70 Minutes
```

This spreads cache expiration over time.

Additional techniques:

* Cache warming
* Serving stale cache

---

# Scenario 4: Cache Penetration

Users repeatedly request invalid data.

Example

```text
Product ID

9999999999
```

Flow

```text
Cache MISS

Database MISS

Next Request

Database Again
```

Attackers can exploit this to overload the database.

---

## Solutions

### Negative Cache

Store NULL results.

```text
NULL

TTL = 30 Seconds
```

Next request

```text
Cache HIT

NULL

No Database Query
```

---

### Bloom Filter

```text
Request
    |
Bloom Filter
    |
Definitely Doesn't Exist
    |
Reject Request
```

The database never receives invalid requests.

---

# Scenario 5: Hot Key

One key suddenly becomes extremely popular.

Example

```text
Virat Kohli Profile
```

Millions of users request

```text
user:virat
```

One Redis node reaches 100% CPU utilization.

---

## Solutions

### Replicate the Hot Key

Instead of

```text
user:virat
```

Store

```text
user:virat:1

user:virat:2

user:virat:3
```

Applications randomly choose one copy.

Load is distributed.

---

### Local Cache

Store popular keys inside application memory.

This significantly reduces Redis traffic.

---

# Scenario 6: Memory Eviction

Redis memory becomes full.

```text
Memory

100%
```

Redis begins removing keys.

Common eviction policies:

```text
LRU

LFU

Random

TTL
```

Choose the eviction policy according to your workload.

---

# Scenario 7: Cache Inconsistency

Database is updated.

Cache still contains old data.

```text
Database

Price = 900

Cache

Price = 1000
```

Users receive stale information.

---

## Solutions

Use proper cache update strategies.

* Cache Aside
* Write Through
* Write Behind
* Write Around

The appropriate strategy depends on consistency and performance requirements.

---

# Scenario 8: Network Failure

Redis is healthy.

The network connection to Redis fails.

```text
Application
      |
Redis Timeout
```

Do not wait indefinitely.

Configure a short timeout.

```text
Timeout

10–20 ms
```

After timeout,

```text
Database
```

The application remains responsive by quickly falling back to the database.
