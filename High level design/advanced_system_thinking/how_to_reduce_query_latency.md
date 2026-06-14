# What is Query Latency?

> Query latency is the total time taken from when a client sends a request until it receives a response.
```
Latency = Network Time + Processing Time + Database Time + External Service Time
```
**Example:**
```
User → API Gateway → Application Server → Database → Response
```
- If the response takes 500 ms, we need to identify where the time is spent and optimize those components.

# 1. Use Caching (Biggest Win)

Caching avoids expensive computations and database queries.

## Types of Caching

### A. Client-side Cache

Store data in the browser or mobile application.

```text
Client Cache
      ↓
     API
```

**Examples:**

* Browser cache
* Mobile local storage
* HTTP cache headers

**Benefits:**

* No network call required
* Near-zero latency

---

### B. CDN Cache

Cache static content close to users.

```text
User
 ↓
CDN Edge
 ↓ (cache miss)
Origin Server
```

**Examples:**

* Images
* CSS
* JavaScript
* Videos

**Benefits:**

* Reduces geographical latency
* Improves content delivery speed

**Common CDN Providers:**

* Cloudflare
* Akamai
* Amazon CloudFront

---

### C. Application Cache

Store frequently accessed data in an in-memory cache.

```text
App
 ↓
Redis
 ↓
Database
```

**Examples:**

* User profiles
* Product details
* Trending videos
* Session data

**Popular Cache Systems:**

* Redis
* Memcached

**Performance Benefit:**

```text
Database Query = 20 ms
Redis Query    = 1 ms
```

Using an application cache can significantly reduce latency and decrease database load, making it one of the most effective techniques for improving query performance.

---

# 2. Optimize Database Queries

A slow query is often the biggest source of latency.

## A. Add Proper Indexes

### How Adding an Index Reduces Query Latency ?

Adding an index reduces latency because it changes **how the database finds the data**.

### Without an Index

Suppose your `users` table has 1,000,000 rows.

```sql
SELECT *
FROM users
WHERE email = 'abc@test.com';
```

If `email` is not indexed, the database performs a **full table scan**:

1. Check row 1
2. Check row 2
3. Check row 3
4. ...
5. Continue until it finds the matching email

This is **O(N)** because the number of checks grows linearly with the table size.

### Example

| Rows      | Maximum Comparisons |
| --------- | ------------------- |
| 1,000     | 1,000               |
| 100,000   | 100,000             |
| 1,000,000 | 1,000,000           |

As the table grows, query latency increases.

---

### With an Index

```sql
CREATE INDEX idx_email
ON users(email);
```

The database creates a data structure (usually a **B-Tree**) that keeps email values sorted.

### Simplified B-Tree Structure

```text
                    m@test.com
                   /          \
         g@test.com          t@test.com
         /      \             /      \
a@test.com j@test.com p@test.com z@test.com
```

When searching for:

```sql
WHERE email = 'abc@test.com';
```

The database:

1. Looks at the root node.
2. Decides which branch contains the value.
3. Moves down one level.
4. Repeats until the record is found.

Instead of scanning every row, it jumps directly to the correct location.

This is approximately **O(log N)**.


---

## B. Avoid Full Table Scans

**Bad:**

```sql
SELECT *
FROM orders;
```

**Good:**

```sql
SELECT id, status
FROM orders
LIMIT 100;
```

Retrieve only the required rows and columns instead of scanning the entire table.

---

## C. Query Optimization

**Bad:**

```sql
SELECT *
```

**Good:**

```sql
SELECT name, email
```

### Benefits

* Less I/O
* Less memory consumption
* Faster serialization and network transfer

Selecting only the required columns reduces the amount of data processed and returned.

---

## D. Denormalized Schema

### Denormalization and Query Performance

Too many joins can significantly increase query latency because the database must combine data from multiple tables before returning the result.

### Normalized Schema

In a normalized database, data is split into separate tables to reduce redundancy.

### Example

* User
* Order
* Product
* Category

To fetch complete order information, the database may need multiple joins:

```sql
SELECT *
FROM orders o
JOIN users u ON o.user_id = u.id
JOIN products p ON o.product_id = p.id
JOIN categories c ON p.category_id = c.id;
```

As the number of joins increases, the database performs more lookups, consumes more CPU and memory, and may read more data from disk. This can increase query latency, especially for large datasets.

---

### Denormalized Schema

In a denormalized design, frequently accessed data is stored together to avoid expensive joins.

### Example

```json
Order {
    order_id,
    user_id,
    product_name,
    category_name,
    quantity,
    price
}
```

Now the application can retrieve most order information directly from the `Order` table without joining other tables.

---

### Why Denormalization Improves Performance

Instead of:

```text
Order → Product → Category
```

the database can read everything from a single table:

```text
Order
```

Benefits include:

* Fewer joins
* Fewer table lookups
* Reduced disk I/O
* Lower CPU overhead
* Faster query execution

This often leads to significantly lower latency for read operations.

---

### Trade-offs

#### Advantages

* Faster reads
* Fewer joins
* Lower query latency
* Better performance for reporting and analytics queries

#### Disadvantages

* Increased storage usage
* Data duplication
* More complex updates
* Risk of inconsistent data if duplicated values are not updated correctly

---

### When to Use Denormalization

Denormalization is commonly used in read-heavy systems where **query performance is more important than storage efficiency.**

Typical examples include:

* E-commerce product catalogs
* Analytics and reporting systems
* Data warehouses
* Search platforms
* High-traffic applications with frequent read operations

A common strategy is to keep transactional databases normalized for data integrity while using denormalized tables or materialized views to optimize read performance.


---

# 3. Use Read Replicas
## Read Replicas for Database Scalability

Read replicas are used to separate read operations from write operations, reducing load on the primary database and improving overall performance.

### Architecture

```text id="k9v4d1"
              ┌─ Replica 1
Primary DB ───┼─ Replica 2
              └─ Replica 3
```

The primary database handles all write operations and continuously replicates data to one or more read replicas.

---

### Request Flow

```text id="m3xq7w"
Writes → Primary Database
Reads  → Read Replicas
```

### Example

When a user places an order:

```sql id="b7n2cf"
INSERT INTO orders (...);
```

The write goes to the primary database.

When users browse products:

```sql id="n8k5tr"
SELECT * FROM products;
```

The read requests are served by replicas.

This prevents read traffic from overwhelming the primary database.

---

### How Read Replicas Reduce Latency

Without replicas:

```text id="j2s8vp"
Primary DB
 ├─ Reads
 ├─ Reads
 ├─ Reads
 └─ Writes
```

The primary database must handle every request, causing resource contention and increased response times.

With replicas:

```text id="x5d1hy"
Primary DB
 └─ Writes

Replica 1
 └─ Reads

Replica 2
 └─ Reads

Replica 3
 └─ Reads
```

Read requests are distributed across multiple servers, reducing the load on the primary database.

As a result:

* Queries wait less time for resources.
* CPU and memory usage are distributed.
* Disk I/O pressure is reduced.
* Read latency decreases.

---

### Benefits

### Lower Query Latency

Read requests are spread across multiple databases instead of competing for resources on a single server.

### Higher Throughput

The system can process many more requests simultaneously because reads are handled in parallel.

### Improved Scalability

Additional replicas can be added as traffic grows without modifying application logic significantly.

### Better Availability

If a replica fails, traffic can be redirected to other replicas while the primary database continues handling writes.

---

### Trade-offs

### Replication Lag

Data changes written to the primary database may take a short time to appear on replicas.

Example:

```text id="z4f7na"
User updates profile
        ↓
Write succeeds on Primary
        ↓
Replica updated a few milliseconds later
```

A user may briefly see stale data when reading from a replica.

### Additional Infrastructure Cost

More database servers mean higher operational and maintenance costs.

### Increased Complexity

Applications need logic or middleware to route writes to the primary database and reads to replicas.

---
# 4. Database Sharding
## Database Sharding for Horizontal Scalability

As data volume grows, a single database server can become a bottleneck. Sharding addresses this problem by splitting data across multiple database servers.

### Before Sharding

All data is stored in a single database.

```text id="q8m4zp"
            One Database
               100 TB
```

As the dataset grows:

* Queries become slower.
* Storage requirements increase.
* CPU and memory resources become constrained.
* More users compete for the same database resources.

This can lead to higher latency and reduced throughput.

---

### After Sharding

The data is partitioned across multiple databases called **shards**.

```text id="r5t2kd"
        ┌─────────┐
        │ Shard 1 │
        └─────────┘

        ┌─────────┐
        │ Shard 2 │
        └─────────┘

        ┌─────────┐
        │ Shard 3 │
        └─────────┘

        ┌─────────┐
        │ Shard 4 │
        └─────────┘
```

Each shard stores only a subset of the total data.

Instead of one database storing 100 TB, each shard may store approximately 25 TB.

---

### Sharding Strategy Example

A common approach is **hash-based sharding**.

```text id="x7n9fw"
Shard Number = User ID % 4
```

#### Distribution

| User ID | User ID % 4 | Shard   |
| ------- | ----------- | ------- |
| 101     | 1           | Shard 1 |
| 102     | 2           | Shard 2 |
| 103     | 3           | Shard 3 |
| 104     | 0           | Shard 4 |
| 105     | 1           | Shard 1 |

When a request arrives, the application calculates the shard using the user ID and routes the query to the correct database.

---

### How Sharding Improves Performance

Without sharding:

```text id="k3p8mz"
All users
      ↓
One Database
```

Every query searches through the same large dataset.

With sharding:

```text id="v6h2qr"
User 101 → Shard 1
User 102 → Shard 2
User 103 → Shard 3
User 104 → Shard 4
```

Queries only access a fraction of the total data.

As a result:

* Smaller indexes
* Fewer rows scanned
* Reduced disk I/O
* Better cache utilization
* Lower query latency

---

### Benefits

### Smaller Datasets Per Node

Each database manages only a portion of the data, making storage and maintenance easier.

### Faster Queries

Queries operate on smaller tables and indexes, reducing execution time.

### Reduced Contention

Users are distributed across multiple databases, decreasing lock contention and resource competition.

### Horizontal Scalability

Capacity can be increased by adding more shards instead of upgrading a single database server.

### Better Resource Utilization

CPU, memory, storage, and network load are distributed across multiple machines.

---

### Trade-offs

### Increased Complexity

Applications must determine which shard contains the required data.

### Cross-Shard Queries

Queries involving data from multiple shards can be difficult and expensive.

Example:

```sql id="m1d7xy"
SELECT *
FROM users
WHERE country = 'India';
```

This query may need to run on every shard and combine the results.

### Re-Sharding Challenges

As data grows, adding new shards may require redistributing existing data, which can be complex and time-consuming.

### Operational Overhead

More databases mean more monitoring, backups, maintenance, and infrastructure management.

---

# 5. Use Asynchronous Processing to Reduce Latency

Not every task needs to be completed before responding to the user. Moving non-critical work to background processing can significantly reduce response times and improve user experience.

## Synchronous Processing (Bad)

In a synchronous workflow, the application waits for every task to finish before returning a response.

```text id="n7w2qx"
Request
  ↓
Save DB
  ↓
Send Email
  ↓
Generate Report
  ↓
Notify User
  ↓
Return Response
```

### Problem

The user must wait for all operations to complete, even if some tasks are not immediately required.

For example:

| Task            | Time   |
| --------------- | ------ |
| Save DB         | 50 ms  |
| Send Email      | 500 ms |
| Generate Report | 2 sec  |
| Notify User     | 100 ms |

Total response time:

```text id="p4z8kr"
≈ 2.65 seconds
```

This increases latency and creates a poor user experience.

---

## Asynchronous Processing (Good)

Only the critical operation is executed during the request.

```text id="m5c1dt"
Request
   ↓
Save DB
   ↓
Return Response
```

Background tasks are processed separately:

```text id="h9x7vb"
Background Queue
   ├─ Send Email
   ├─ Generate Report
   └─ Notify User
```

The user receives a response almost immediately while background workers handle the remaining tasks.

---

### Typical Architecture

```text id="r3j6kw"
API
 ↓
Message Queue
 ↓
Worker
```

#### Flow

1. User sends a request.
2. API performs essential work (e.g., save data).
3. API publishes a message to a queue.
4. API returns a response immediately.
5. Background workers consume messages from the queue.
6. Workers execute time-consuming tasks asynchronously.

---

### Example: E-Commerce Order Placement

#### Synchronous

```text id="t8v4hy"
Place Order
   ↓
Save Order
   ↓
Send Confirmation Email
   ↓
Update Inventory
   ↓
Generate Invoice
   ↓
Return Response
```

#### Asynchronous

```text id="q6k2mp"
Place Order
   ↓
Save Order
   ↓
Return Response
```

Background processing:

```text id="f7n9cx"
Queue
 ├─ Send Confirmation Email
 ├─ Generate Invoice
 └─ Update Analytics
```

The order is successfully placed immediately, while secondary tasks execute in the background.

---


### Benefits

### Lower User-Perceived Latency

Users receive responses immediately instead of waiting for all processing to complete.

### Better Scalability

Background workers can be scaled independently based on workload.

### Improved Throughput

APIs spend less time processing requests and can serve more users concurrently.

### Fault Tolerance

Failed background jobs can be retried without affecting the original user request.

### Decoupled Architecture

Services become more independent and easier to maintain.

---

### Trade-offs

### Eventual Consistency

Background tasks may not complete immediately, so some data may take time to become fully updated.

### Increased Complexity

Queues, workers, retries, monitoring, and error handling must be managed.

### Debugging Challenges

Tracking failures across distributed systems can be more difficult than in synchronous workflows.

---

### When to Use Asynchronous Processing

Asynchronous processing is ideal for tasks that are not required to complete before responding to the user, such as:

* Sending emails
* SMS notifications
* Push notifications
* Report generation
* Image and video processing
* Analytics updates
* Audit logging
* Data synchronization
* Machine learning inference jobs

By moving non-critical operations to background workers, systems can dramatically reduce user-perceived latency while improving scalability and overall performance.

---

# 6. Reduce Network Latency

Sometimes application logic and database queries are fast, but overall response time is still high because of network delays. Optimizing network communication can significantly improve system performance and user experience.

---

## A. Keep Services Close to Users

The physical distance between users and services directly affects latency.

### Bad Architecture

```text id="m8v2pk"
User (India)
     ↓
API Server
     ↓
Database (US)
```

Every request must travel thousands of kilometers, increasing round-trip time.

### Better Architecture

```text id="k3r7xh"
India User
     ↓
India Region
     ↓
India Database Replica
```

Users connect to services located in the same geographic region, reducing network travel distance.

#### Benefits

* Lower round-trip time (RTT)
* Faster page loads
* Improved API response times
* Better user experience

#### Common Solutions

* Multi-region deployments
* Regional database replicas
* Content Delivery Networks (CDNs)
* Edge computing

---

## B. Compression

Large payloads take longer to transfer across the network.

### Without Compression

```text id="p6j1fn"
Response Size: 1 MB
```

The full payload must be transmitted over the network.

### With Compression

```text id="s4n8qy"
Response Size: 100 KB
```

The payload is compressed before transmission and decompressed by the client.

### Popular Compression Algorithms

#### Gzip

Widely supported and commonly used for:

* HTML
* CSS
* JavaScript
* JSON responses

#### Brotli

Modern compression algorithm that typically achieves better compression ratios than Gzip.

Commonly used for:

* Web applications
* Static assets
* API responses

#### Benefits

* Reduced bandwidth usage
* Faster downloads
* Lower transfer times
* Improved performance on slower networks

Example:

```text id="d7w3lz"
1 MB → 100 KB
```

Transferring 100 KB is significantly faster than transferring 1 MB.

---

## C. Persistent Connections

Establishing a new connection for every request introduces unnecessary overhead.

### Without Persistent Connections

Each request requires:

```text id="t2h9vc"
TCP Handshake
      ↓
TLS Handshake
      ↓
Send Request
      ↓
Receive Response
```

Repeating these steps for every request increases latency.

### With Persistent Connections

```text id="r5x1md"
TCP/TLS Connection
        ↓
Request 1
Request 2
Request 3
Request 4
```

The connection remains open and is reused for multiple requests.

---

## Why Network Optimization Matters ?

Consider a request where:

```text id="q9k4rb"
Application Processing: 20 ms
Database Query: 30 ms
Network Delay: 250 ms
```

Total latency:

```text id="f1z7ne"
300 ms
```

Even though computation takes only 50 ms, network delays account for most of the response time.

Optimizing the network can often provide larger performance gains than optimizing application code.

---

### Benefits of Reducing Network Latency

### Faster Response Times

Users receive results more quickly.

### Better Scalability

Less bandwidth and fewer connection resources are consumed.

### Improved User Experience

Applications feel more responsive and interactive.

### Reduced Infrastructure Costs

Efficient data transfer lowers bandwidth usage and resource consumption.

---

# 7. Scale Horizontally

As application traffic grows, a single server eventually becomes a bottleneck. Horizontal scaling solves this problem by distributing requests across multiple servers.

## Single Server Architecture

```text id="h4k7mx"
        Application Server
              │
              ▼
         1000 req/s
```

A single server can only handle a limited number of requests per second.

As traffic increases:

* CPU utilization rises
* Memory consumption increases
* Request queues grow
* Response times become slower

Eventually, the server reaches its capacity and becomes a bottleneck.

---

## Horizontal Scaling with Load Balancing

Instead of relying on one server, multiple application instances are deployed behind a load balancer.

```text id="p8v3nr"
        Load Balancer
          /   |   \
      App1 App2 App3
```

The load balancer distributes incoming requests across available servers.

### Request Flow

```text id="j2r6wy"
User Request
      ↓
Load Balancer
      ↓
App1 / App2 / App3
```

Each server handles only a portion of the total traffic.

---

## How Horizontal Scaling Reduces Latency ?

### Without Load Balancing

```text id="q5m9cb"
1000 Users
      ↓
Single Server
```

When traffic spikes:

* Requests wait in queues
* CPU becomes overloaded
* Response times increase

Example:
```
| Incoming Traffic | Server Capacity |
| ---------------- | --------------- |
| 800 req/s        | 1000 req/s      |
| 1500 req/s       | 1000 req/s      |
```
The extra 500 requests must wait, increasing latency.

---

### With Load Balancing

```text id="w7n4kx"
1000 Users
      ↓
Load Balancer
   ↙   ↓   ↘
App1 App2 App3
```

If each server handles:

```text id="c9t2fz"
1000 req/s
```

Then:

```text id="b3k8qh"
3 Servers
≈ 3000 req/s Capacity
```

Traffic is distributed evenly, reducing overload and improving response times.

---

### Benefits

### Lower Queueing Delay

Requests spend less time waiting because workload is spread across multiple servers.

### Better Response Time

Servers process requests faster when they are not overloaded.

### Higher Throughput

The system can handle significantly more requests per second.

### Improved Availability

If one server fails, traffic can be routed to healthy instances.

### Easier Scalability

Additional servers can be added as demand increases.

---

## Load Balancing Algorithms

### Round Robin

Requests are distributed sequentially.

```text id="r6p1dv"
Request 1 → App1
Request 2 → App2
Request 3 → App3
Request 4 → App1
```

### Least Connections

Traffic is sent to the server with the fewest active connections.

### Weighted Load Balancing

More powerful servers receive a larger share of traffic.

---

# 8. Optimize Microservice Communication

Microservices improve scalability and maintainability, but excessive service-to-service communication can significantly increase latency.

### The Latency Problem

A common microservice architecture looks like this:

```text id="v4m8pz"
API
 ↓
Service A
 ↓
Service B
 ↓
Service C
 ↓
Service D
```

Each service call introduces:

* Network latency
* Serialization/deserialization overhead
* Processing time
* Connection setup costs

Even if each service is fast, the delays accumulate.

### Example

```text id="q7n2rx"
Service A → B = 20 ms
Service B → C = 20 ms
Service C → D = 30 ms
Service D Processing = 40 ms
```

Total latency:

```text id="k9p5wf"
20 + 20 + 30 + 40
= 110 ms
```

As more services are added, response times continue to increase.

---

## Solution 1: Parallel Calls

### Sequential Execution

Services are called one after another.

```text id="h3x8tb"
A → B → C
```

Latency:

```text id="m6r1qy"
A waits for B
B waits for C
```

Total latency:

```text id="n4z7dk"
Time(B) + Time(C)
```

---

### Parallel Execution

If services are independent, execute them simultaneously.

```text id="p2v9jc"
      B
     /
A ----
     \
      C
```

Example:

```text id="g8k4mz"
Service B = 50 ms
Service C = 40 ms
```

Sequential:

```text id="r5d1wp"
50 + 40
= 90 ms
```

Parallel:

```text id="t7n3bx"
max(50, 40)
= 50 ms
```

The total response time becomes the duration of the slowest request instead of the sum of all requests.

---

### Example Using Parallel Requests

#### Sequential

```text id="w9m6ph"
Get User Profile
      ↓
Get Orders
      ↓
Get Recommendations
```

Total:

```text id="s3k8vr"
30 + 40 + 50
= 120 ms
```

#### Parallel

```text id="f1p7zn"
            ┌─ Get Orders
Get Profile ┤
            └─ Get Recommendations
```

Total:

```text id="j6r2xd"
max(40, 50)
= 50 ms
```

This significantly reduces overall latency.

---

## Solution 2: Aggregation Layer

Clients often need data from multiple services.

### Without Aggregation

```text id="d4q8kt"
Client
 ├─ User Service
 ├─ Order Service
 ├─ Product Service
 └─ Recommendation Service
```

Problems:

* Multiple network round trips
* Increased client complexity
* Higher latency

---

### With Aggregation Layer

```text id="c7m5yb"
Client
  ↓
API Gateway
  ↓
Aggregator
```

The aggregator communicates with backend services and combines responses into a single payload.

```text id="x2r9pn"
Aggregator
 ├─ User Service
 ├─ Order Service
 ├─ Product Service
 └─ Recommendation Service
```

The client receives one consolidated response.

---

### Benefits of Aggregation :

### Fewer Network Round Trips

Instead of multiple client requests:

```text id="v8k3qt"
Client → Service 1
Client → Service 2
Client → Service 3
```

Use:

```text id="b5n7rh"
Client → Aggregator
```

This reduces latency and bandwidth usage.

### Simpler Clients

Frontend applications only need to call a single endpoint.

### Better Performance

The aggregator can perform parallel calls internally and return a combined response.

---

# 9. Precomputation for Low-Latency Systems

Some computations are expensive and take significant time to execute. Performing these calculations on every request can dramatically increase latency.

Precomputation solves this problem by calculating results ahead of time and storing them for fast retrieval.

---

## The Problem

Suppose a system needs to display trending videos.

### Without Precomputation

```text id="p4m8zx"
User Request
      ↓
Compute Trending Videos
      ↓
Scan Millions of Records
      ↓
Calculate Rankings
      ↓
Return Response
```

Every request triggers expensive computations.

This may involve:

* Large database scans
* Aggregations
* Sorting operations
* Machine learning inference
* Ranking algorithms

As traffic increases, latency grows significantly.

---

## The Solution: Precompute Results

Instead of calculating rankings on every request:

```text id="n7k3qy"
Cron Job
    ↓
Precompute Rankings
    ↓
Store Results
```

User requests become:

```text id="j2v9rd"
User Request
      ↓
Read Precomputed Data
      ↓
Return Response
```

The expensive computation is moved out of the request path.

---

### Example: Trending Videos

### Real-Time Computation

```text id="r5x1mc"
Request
   ↓
Count Views
   ↓
Calculate Popularity Score
   ↓
Sort Videos
   ↓
Return Top 100
```

Latency may be:

```text id="f8n4kt"
500 ms – 5 sec
```

depending on data size.

---

### Precomputed Approach

A scheduled job runs periodically.

```text id="w6p2zb"
Every 5 Minutes
      ↓
Calculate Trending Videos
      ↓
Store Top 100
```

User requests simply fetch the results:

```text id="c9m7dx"
SELECT *
FROM trending_videos;
```

Latency may drop to:

```text id="b3r8qn"
5–20 ms
```

because no heavy computation is required.

---

## Typical Architecture

```text id="t4k6wp"
Background Job
      ↓
Compute Results
      ↓
Database / Cache
      ↓
API
      ↓
Users
```

The API serves precomputed data rather than generating it in real time.

---

### Common Precomputation Techniques

### Scheduled Jobs (Cron Jobs)

Run computations at fixed intervals.

```text id="m1v7ry"
Every Minute
Every Hour
Every Day
```

Examples:

* Trending content
* Daily reports
* Analytics summaries

---

### Materialized Views

Store the result of complex database queries.

```sql id="q8p4zn"
CREATE MATERIALIZED VIEW top_products AS
SELECT product_id,
       COUNT(*) AS sales
FROM orders
GROUP BY product_id;
```

The database periodically refreshes the view.

Benefits:

* Faster queries
* Reduced database workload

---

### Batch Processing

Large datasets are processed offline.

```text id="g5x2kc"
Raw Data
   ↓
Batch Processing
   ↓
Aggregated Results
```

Commonly used in:

* Data warehouses
* Business intelligence systems
* Analytics platforms

---

### Cached Computations

Store expensive calculations in memory.

```text id="h9m3wr"
Compute Once
     ↓
Store in Cache
     ↓
Reuse Many Times
```

This avoids repeated computation.

---

### Benefits

### Lower Latency

Requests simply read existing results instead of performing expensive calculations.

### Reduced CPU Usage

Heavy computations are executed less frequently.

### Higher Throughput

Servers can handle more requests because less processing is required.

### Better Scalability

Traffic spikes have less impact because expensive operations are not performed per request.

---

### Trade-offs

### Stale Data

Precomputed results may not reflect the most recent updates.

Example:

```text id="k7r1vd"
Trending List Updated
Every 5 Minutes
```

Users may see rankings that are a few minutes old.

---

### Additional Storage

Precomputed results require storage space.

---

### Background Processing Costs

Scheduled jobs consume compute resources and require monitoring.

---

## Interview-Ready Answer: How to Reduce Query Latency

To reduce query latency, I would first identify the bottleneck using metrics, monitoring, and distributed tracing. Once the latency hotspots are identified, I would optimize the system across multiple layers:

1. **Implement Caching**

   * Use browser caching, CDN caching, and application-level caching (e.g., Redis) to reduce database hits and computation overhead.

2. **Optimize Database Queries**

   * Add proper indexes, tune slow queries, avoid unnecessary joins, and use denormalization where appropriate to improve read performance.

3. **Use Read Replicas and Sharding**

   * Route read traffic to replicas and shard large datasets to distribute load and reduce query execution time.

4. **Move Non-Critical Tasks to Asynchronous Processing**

   * Offload operations such as email notifications, report generation, and logging to message queues and background workers.

5. **Reduce Network Overhead**

   * Use compression, connection pooling, HTTP/2 or HTTP/3, and geo-distributed deployments to minimize network latency.

6. **Scale Horizontally**

   * Add more application servers behind a load balancer to reduce request queuing and improve response times.

7. **Optimize Microservice Communication**

   * Minimize service-to-service hops, aggregate APIs where possible, and parallelize independent service calls.

8. **Precompute Expensive Results**

   * Calculate frequently requested data in advance and store the results for quick retrieval instead of recomputing them for every request.

Finally, I would continuously monitor latency metrics such as P50, P95, and P99 to ensure that optimizations are effective and to identify new bottlenecks as the system scales.
