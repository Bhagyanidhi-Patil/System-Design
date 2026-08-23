# How to Delete Billions of WhatsApp Statuses

How do you delete billions of WhatsApp Statuses every day without bringing your database to its knees? A simple cron job will not scale.

The real challenge is designing an automatic, distributed expiration system that handles massive writes, high traffic, and cleanup efficiently. TTL, asynchronous workers, partitioning, and distributed systems are the key ideas.

## 1. Understand the Problem

WhatsApp Status behaves roughly like this:

- A user uploads a status.
- Other users can view it.
- The status is valid for 24 hours.
- After 24 hours, it should no longer be visible.
- Eventually, the underlying data should be deleted.

The naive solution is:

```text
Every night at 12 AM:
    Find expired statuses
    DELETE them
```

This does not scale. Imagine one billion statuses per day. If all expired statuses are scanned and deleted together, the database experiences:

- Huge database load
- Disk I/O spikes
- CPU spikes
- Lock contention
- Replication lag
- Cache invalidations
- Massive write amplification
- Possible outage

The key idea is to distribute expiration work across time and machines instead of processing it as one giant batch.

## 2. Requirements

### Functional requirements

- Store a status.
- Make it available for 24 hours.
- Automatically expire it after 24 hours.
- Ensure expired statuses are not visible.
- Eventually delete the underlying data.

### Non-functional requirements

- Massive scale
- High availability
- Low latency for status reads
- No large deletion spikes
- Horizontal scalability
- Fault tolerance
- Retry support
- Idempotent deletion

## 3. Expiration and Physical Deletion

Expiration and physical deletion are two different operations.

```text
10:00 AM             User uploads Status A
10:00 AM next day    Status A expires
10:00 AM + minutes   Physical data is deleted
```

The data does not need to be physically deleted at the exact expiration second. The system can first make it logically expired and then clean it up asynchronously.

## 4. High-Level Architecture

```text
                           +--------+
                           | Client |
                           +---+----+
                               |
                               v
                         +-----------+
                         | Status API|
                         +-----+-----+
                               |
                  +------------+------------+
                  |                         |
                  v                         v
          +---------------+         +---------------+
          | Status Store  |         | Object Storage |
          | Metadata      |         | Image / Video  |
          +-------+-------+         +---------------+
                  |
                  | expiration event
                  v
          +------------------+
          | Expiration Queue |
          | / Kafka          |
          +--------+---------+
                   |
             +-----+-----+-----+
             |           |     |
             v           v     v
          Worker 1    Worker 2  Worker 3
             |           |     |
             +-----+-----+-----+
                   v
          +------------------+
          | Delete Service   |
          +--------+---------+
                   |
             +-----+------+
             |            |
             v            v
       Metadata DB   Object Storage
```

## 5. Status Storage

Suppose a user uploads a status:

```text
status_id  = S123
user_id    = U456
created_at = 10:00 AM
expires_at = 10:00 AM next day
media_url  = ...
```

The database stores metadata:

```text
Status {
    status_id
    user_id
    media_url
    created_at
    expires_at
}
```

The actual image or video should preferably live in object storage while the database contains metadata.

```text
Database
--------------------------------
S123 | U456 | expires_at
S124 | U789 | expires_at

Object Storage
--------------------------------
S123 -> image/video
S124 -> image/video
```

## 6. How Do We Detect Expiration?

### Approach 1: Database scanning

Every minute, the system could run:

```sql
SELECT *
FROM statuses
WHERE expires_at < NOW();
```

At massive scale, repeated scans over billions of rows make the database a bottleneck. Even with an index, the deletion work is enormous.

The database should not be responsible for constantly discovering expiration.

## 7. Delayed Messages

When a status is created:

```text
Status created
      |
      v
expires_at = T + 24 hours
      |
      v
Schedule expiration event
```

The expiration system eventually produces `ExpireStatus(S123)` at approximately `T + 24 hours`.

This is better than repeated database scans, but creating one independent timer per status still does not scale to billions of statuses.

## 8. Distributed Expiration Scheduler

Use a time-bucketed scheduler instead of one timer per status.

```text
10:00 - 10:01 -> Bucket A
10:01 - 10:02 -> Bucket B
10:02 - 10:03 -> Bucket C
```

For example:

```text
Status A -> expires 10:03:15
Status B -> expires 10:03:20
Status C -> expires 10:03:55
```

All three statuses belong to the `10:03` expiration bucket:

```text
10:03 bucket
  - Status A
  - Status B
  - Status C
```

This is much more scalable than maintaining billions of independent timers.

## 9. Time Buckets and Partitioning

If one minute contains millions of statuses, one worker should not process the entire bucket. Partition it further:

```text
Bucket: 10:03

Partition 0 -> statuses 0-999999
Partition 1 -> statuses ...
Partition 2 -> statuses ...
Partition 3 -> statuses ...
```

Alternatively, partition by status ID:

```text
partition = hash(status_id) % N
```

```text
                    10:03 Bucket
                 /        |        \
                v         v         v
          Partition 0  Partition 1  Partition 2
                |         |         |
                v         v         v
             Worker 1   Worker 2   Worker 3
```

This allows horizontal scaling.

## 10. Kafka or a Distributed Queue

A practical implementation can use a distributed log or queue such as Kafka.

```text
Expiration Scheduler
          |
          v
        Kafka
       /  |  \
      v   v   v
     P0  P1  P2
      |   |   |
      v   v   v
     W1  W2  W3
```

An expiration message could contain:

```json
{
  "status_id": "S123",
  "user_id": "U456",
  "expires_at": "10:03:15"
}
```

Kafka provides:

- Partitioning
- Horizontal scalability
- Consumer groups
- Durability
- Replay
- Ordering within a partition
- Backpressure

## 11. What Happens When a Status Expires?

Suppose `S123` is created at 10:00 AM and expires at 10:00 AM the next day.

### Step 1: Upload

```text
Client -> Status API -> Metadata DB -> Object Storage
```

### Step 2: Schedule expiration

The expiration scheduler places `S123` in the bucket for 10:00 AM the next day.

### Step 3: Publish the expiration event

When the bucket is due, the scheduler publishes `ExpireStatus(S123)` to Kafka.

### Step 4: Consume and verify

The worker checks whether the status is actually expired:

```text
current_time >= expires_at
```

This check matters because messages may be delayed. For example, an event expected at 10:00 may be received at 10:02.

## 12. Logical Expiration

The read path must ignore expired statuses even if physical deletion has not completed:

```sql
SELECT *
FROM statuses
WHERE user_id = ?
  AND expires_at > NOW();
```

Therefore, user-visible expiration does not depend on physical deletion finishing immediately.

## 13. Physical Deletion

After expiration, the worker deletes both metadata and media:

```text
Expiration Worker
       |
       +-> Delete metadata
       |
       +-> Delete media
```

For example:

```sql
DELETE FROM status_metadata
WHERE status_id = 'S123';
```

Deleting billions of objects individually can also be expensive, so object-storage cleanup should support asynchronous processing and batching.

## 14. Batch Deletion

Instead of deleting one row per operation, accumulate a small batch:

```text
Worker -> 1000 expired statuses -> Batch delete
```

Batching reduces network calls, transaction overhead, and database round trips. The batch size must be controlled; one billion rows should never be processed in one transaction.

## 15. Rate Limiting and Backpressure

Suppose expiration workers produce 100K deletes per second while the database can handle only 50K deletes per second. Add a deletion queue and rate limiter:

```text
Workers -> Deletion Queue -> Rate Limiter -> Database
```

The queue absorbs excess work while the database catches up. This is a classic distributed-systems backpressure pattern.

## 16. Database Sharding

A single database may not be able to handle billions of statuses. Shard metadata, for example, using:

```text
hash(user_id) % N
```

```text
             Status Service
             /      |      \
            v       v       v
          DB-1    DB-2    DB-3
```

Expiration workers then distribute deletes across shards instead of sending all traffic to one database.

## 17. The Hot Partition Problem

A hot partition occurs when one partition receives substantially more work than the others:

```text
DB 0 -> 5%
DB 1 -> 5%
DB 2 -> 5%
DB 3 -> 85%  (hot)
```

If partitioning uses `user_id % 4`, a celebrity user's workload may be concentrated on one partition. That partition becomes overloaded while the others remain mostly idle.

The same issue can affect expiration events:

```text
partition = hash(user_id) % 4

Partition 0 -> 10 million expiration events
Partition 1 -> 2 million
Partition 2 -> 2 million
Partition 3 -> 2 million
```

Worker 0 becomes the bottleneck. Overload can cause high latency, timeouts, retries, and even more traffic.

### Solution

For expiration work, use a more evenly distributed key:

```text
partition = hash(status_id) % N
```

Hashing status IDs spreads expiration work across workers and reduces the risk of hot partitions.

## 18. Worker Failure

The queue must retain an expiration event until processing succeeds.

```text
Kafka -> Worker -> Delete succeeds -> Commit offset
```

If a worker crashes before committing the offset, Kafka makes the message available again for another worker.

## 19. Idempotency

The same expiration event may be processed more than once:

```text
Worker 1 -> Deletes S123 -> Crashes before ACK
Worker 2 -> Receives S123
```

Deletion should remain safe when `S123` is already deleted. Design for at-least-once processing with idempotent operations rather than assuming exactly-once execution.

## 20. Retry Strategy

If object deletion fails, use exponential backoff:

```text
1 sec -> 2 sec -> 4 sec -> 8 sec -> 16 sec -> ...
```

After several failed attempts, move the event to a Dead Letter Queue for investigation or later replay.

## 21. Scheduler Failure

Avoid a single scheduler. Run multiple schedulers with partition ownership:

```text
Bucket 1 -> Scheduler 1
Bucket 2 -> Scheduler 2
Bucket 3 -> Scheduler 3
```

If Scheduler 1 fails, a coordinator or consensus mechanism detects the failure and assigns its buckets to Scheduler 2. This avoids a single point of failure.

## 22. Duplicate Scheduling

Distributed schedulers may accidentally produce duplicate events:

```text
ExpireStatus(S123)
ExpireStatus(S123)
```

This is acceptable when deletion is idempotent. Avoid the complexity of guaranteeing exactly-once scheduling when an at-least-once design with an idempotent consumer provides the required behavior.

## 23. Time-Based Database Partitions

Instead of storing every status in one giant table, partition by expiration time:

```text
status_2026_08_20
status_2026_08_21
status_2026_08_22
status_2026_08_23
```

Once an entire partition has expired, dropping it can be much cheaper than deleting billions of individual rows:

```sql
DROP PARTITION status_2026_08_20;
```

This optimization is especially useful when the database supports efficient time-based partitioning.

## 24. Final Architecture with Time Partitions

The approaches can be combined:

```text
                         Status API
                       /           \
                      v             v
             Metadata DB       Object Storage
             time-sharded
                    |
                    v
          Expiration Scheduler
                    |
                    v
              Time Buckets
                    |
                    v
                  Kafka
                /  |  \
               v   v   v
              W1  W2  W3
                \  |  /
                    v
             Deletion Service
                /        \
               v          v
          Metadata DB  Object Store
```

### Central design principles

- Separate logical expiration from physical deletion.
- Use time buckets instead of one timer per status.
- Partition work across queues and workers.
- Use status IDs to distribute expiration work evenly.
- Apply batching, rate limiting, and backpressure.
- Design for worker failure, retries, duplicates, and idempotency.
- Use time-based database partitions when dropping partitions is cheaper than row-by-row deletion.
