# Where would you place cache in your design? 
## 1. First Principle: Cache Close to the Consumer

A general rule:

> The closer the cache is to the user, the lower the latency and the less load on downstream systems.

Imagine you order coffee every day:

- If the coffee is already on your desk → instant.
- If it's in the office kitchen → a short walk.
- If it's at a nearby café → a longer walk.
- If it's at a warehouse across town → much slower.
Computer systems work the same way.

Typical request flow:

```
User
  |
Browser Cache
  |
CDN Cache
  |
Load Balancer
  |
Application Cache
  |
Database Cache
  |
Database
```

Each layer solves a different problem.

---

## 2. Browser Cache (Client-Side Cache)

### What is it?

A **browser cache** is storage inside the user's browser that keeps copies of files previously downloaded from a website, such as images, CSS, and JavaScript files. When the user visits the website again, the browser can reuse these local copies instead of downloading them again.

```text
User Browser
     |
     +-- Cached Images
     +-- Cached CSS
     +-- Cached JavaScript
```

Instead of downloading the same files every time, the browser reuses the local copy.

---

### What Should Be Cached?

Files that rarely change:

* Images (`logo.png`, icons)
* CSS files (`styles.css`)
* JavaScript bundles (`app.js`)
* Fonts
* Other static assets

---

### Why Cache Them?

### 1. Zero Server Hit

Without cache:

```text
Browser --> Server --> logo.png
```

With cache:

```text
Browser --> Local Cache --> logo.png
```

The request never reaches the server.

---

### 2. Fastest Possible Response

Reading from local storage is much faster than downloading over the internet.

```text
Local Disk/Memory  <<<<<<  Internet Request
```

Pages load almost instantly.

---

### 3. Reduces Bandwidth Cost

If 1 million users download a 1 MB file:

```text
1 MB × 1,000,000 = 1 TB
```

With browser caching, users download it once and reuse it many times.

---

### YouTube Example

When you open YouTube for the first time:

```text
Download:
- logo.png
- app.js
- styles.css
```

The browser stores these files.

Next time you visit YouTube:

```text
Use local cache:
- logo.png ✓
- app.js ✓
- styles.css ✓
```

Only new or changed content (videos, comments, recommendations) needs to be fetched.

---

### Real-Life Analogy

Think of browser cache like keeping a textbook on your desk.

* First time → go to the library and get the book.
* Later → use the copy on your desk.

You don't keep walking to the library for the same book every day.

---

### Key Idea

Browser cache is the **first and fastest cache layer** because it lives directly on the user's device.

If the browser already has the file, the request doesn't even leave the computer, resulting in near-instant loading and zero server load.

---

## 3. CDN Cache (Edge Cache)

### What is it?

A **CDN (Content Delivery Network) Cache** stores copies of content on servers located around the world, closer to users.

It sits between users and the application's backend servers.

```text
Users
   |
  CDN
   |
 Backend
```

Instead of every request reaching the backend, the CDN can serve cached content directly from a nearby edge server.

---

### Popular CDNs

* Cloudflare
* Akamai
* Amazon CloudFront

---

### What Should Be Cached?

Content that is requested frequently and does not change often:

* Images
* Videos
* CSS and JavaScript files
* Static web pages
* API responses (when appropriate)

---

### Example

Consider an Instagram profile picture:

```text
cdn.instagram.com/profile123.jpg
```

When millions of users request the same image, the CDN can serve it directly from its edge locations instead of forwarding every request to Instagram's backend servers.

---

### Benefits

### 1. Reduces Backend Traffic

Many requests are handled by the CDN, reducing the load on application servers and databases.

### 2. Global Low Latency

Users receive content from the nearest CDN location rather than a distant origin server.

### 3. Better Scalability

During traffic spikes, the CDN absorbs a large portion of requests, helping the system handle millions of users efficiently.

---

### Real-Life Analogy

Think of a CDN as a network of local warehouses.

Without a CDN:

```text
Customer --> Central Warehouse
```

With a CDN:

```text
Customer --> Nearest Local Warehouse
```

Getting items from a nearby warehouse is much faster than shipping everything from a single central location.

---

## 4. Application Cache (Redis/Memcached Cache)

### What is it?

An **Application Cache** is a cache placed close to the application servers. It stores frequently accessed data in memory so the application can avoid repeatedly querying the database.

```text
App Server
    |
  Redis
```

This is one of the most commonly discussed caching layers in system design interviews.

---

### Popular Technologies

* Redis
* Memcached

---

### What Should Be Cached?

Data that is read frequently but changes relatively infrequently:

* User profiles
* Product information
* Trending posts
* Session data
* Configuration data
* Popular API responses

---

### Example

Suppose the application needs user information:

```sql
SELECT * FROM users WHERE id = 100;
```

Instead of querying the database every time, the application can store the result in Redis:

```text
user:100
```

Future requests can retrieve the data directly from Redis.

---

### Why Use Application Cache?

### Faster Response Times

Typical response times:

```text
Database: 10–100 ms
Redis: < 1 ms
```

Since Redis stores data in memory, it is significantly faster than reading from a database.

### Reduced Database Load

Thousands of repeated requests can be served from Redis instead of hitting the database.

### Improved Scalability

The application can handle more users because the database receives fewer requests.

---

### Request Flow

```text
Request
   |
Check Redis
   |
Hit -----> Return Data
   |
Miss
   |
Database
   |
Store in Redis
   |
Return Data
```

### Cache Hit

The requested data exists in Redis.

```text
Request
   |
Redis
   |
Return Data
```

No database query is needed.

### Cache Miss

The requested data is not in Redis.

```text
Request
   |
Redis (Miss)
   |
Database
   |
Store in Redis
   |
Return Data
```

The application fetches the data from the database, stores it in Redis, and returns the result.

---

### Cache-Aside Pattern

This approach is called the **Cache-Aside Pattern**.

Steps:

1. Check the cache.
2. If data exists, return it.
3. If data does not exist, fetch from the database.
4. Store the result in the cache.
5. Return the data to the user.

```text
Read Request
     |
Check Cache
     |
Hit --> Return
     |
Miss
     |
Database
     |
Update Cache
     |
Return
```

This is the most common caching strategy used in real-world applications and is a favorite topic in system design interviews.

---

## When Is a Distributed Cache Used?

A **distributed cache** is used when you have **multiple application servers** and need a shared cache.

### Single Server → Local Cache Is Enough

```text
Users
  |
App Server
  |
Database
```

You can keep data in memory:

```text
App Server
   |
Local Cache (Caffeine, HashMap, etc.)
```

No need for Redis.

---

### Multiple Servers → Distributed Cache

```text
          Load Balancer
                |
      -------------------
      |        |        |
    App1     App2     App3
      \        |        /
           Redis
             |
         Database
```

Now users can hit any server.

If App1 caches `user:100` locally:

```text
App1 -> user:100 ✓
App2 -> user:100 ✗
App3 -> user:100 ✗
```

Each server has a different cache state.

A distributed cache solves this:

```text
App1 ----\
App2 ----- Redis
App3 ----/
```

All servers read the same cached data.

---

## 5. Database Cache (Buffer Cache)

### What is it?

Most modern databases maintain an **internal cache** that stores frequently accessed data in memory.

This cache exists inside the database layer and is managed automatically by the database engine.

```text id="g2zv5n"
Request
   |
Application
   |
Redis (Distributed Cache)
   |
Cache Hit? ---- Yes --> Return
   |
   No
   |
Database Buffer Cache
   |
Cache Hit? ---- Yes --> Return
   |
   No
   |
Disk
```

**In a typical system, the database cache (buffer pool) is checked only after the distributed cache (Redis/Memcached) misses.**

---

### Examples

Popular database cache implementations include:

* MySQL InnoDB Buffer Pool
* PostgreSQL Shared Buffers
* SQL Server Buffer Cache
* Oracle Database Buffer Cache

---

### Purpose

The primary goal is to keep **hot data** (frequently accessed rows and pages) in memory.

Instead of repeatedly reading data from disk, the database can serve requests directly from RAM.

---

### How It Works

Suppose an application executes:

```sql id="s3htgv"
SELECT * FROM users WHERE id = 100;
```

### First Request

```text id="w1wjrf"
Application
    |
 Database
    |
  Disk
```

The database reads the required page from disk and stores it in the buffer cache.

### Subsequent Requests

```text id="9y4dxk"
Application
    |
Database Buffer Pool
    |
 Return Data
```

The data is served directly from memory without accessing disk.

---

### Benefits

### 1. Avoids Disk Reads

Disk access is significantly slower than memory access.

```text id="3az8gd"
Memory Access  <<<<<<  Disk Access
```

Serving data from memory improves performance dramatically.

### 2. Faster Query Execution

Frequently accessed records can be returned much more quickly when they are already cached.

### 3. Reduced Disk I/O

Less reading from disk means lower storage overhead and better database throughput.

---

### Difference Between Redis and Database Cache

### Redis (Application Cache)

```text id="3iqejk"
Application
    |
  Redis
    |
 Database
```

* Managed by the application
* Shared across application servers
* Stores selected data explicitly
* Extremely fast

### Database Cache

```text id="w0m3qy"
Application
    |
 Database
    |
 Buffer Pool
    |
   Disk
```

* Managed automatically by the database
* Stores recently or frequently accessed pages
* Transparent to the application
* Helps reduce disk access

---

## 6. Search Cache

- A **Search Cache** stores the results of popular search queries so that the search engine does not need to execute the same query repeatedly.
- Search cache is usually a server-side cache, not a browser cache.

- It is typically **placed in front of the search cluster.**

```text id="evxw7z"
User
  |
Cache
  |
Elasticsearch
```

When a user performs a search, the system first checks the cache. If the result already exists, it is returned immediately without querying Elasticsearch.

### What is a Search Cluster?

> A search cluster is a group of search servers running technologies such as:

- Elasticsearch
- OpenSearch
- Apache Solr

### Search Request Flow

When a user searches for:
```
iphone
```
the flow is:
```
User
  |
Search API
  |
Redis Cache
  |
Elasticsearch
```
- Search API receives the query.
- Check Redis for search:iphone.
- If found → return cached results.
- If not found → query Elasticsearch.
- Store results in Redis.
- Return results to the user.

### A database and a search cluster are usually separate systems.
Normal Data Lookup

If you're fetching a user by ID:
```
SELECT * FROM users WHERE id = 100;
```
Search Query

If you're searching:
```
iphone
```
### Why Have Elasticsearch?

Imagine an Amazon-like product table:
```
Products
--------------------------------
1 | iPhone 16 Pro
2 | iPhone 16
3 | Samsung Galaxy S26
```
Searching:
```
iphone
```
directly in the database is expensive at scale.

Instead:
- Product data is copied from the database into Elasticsearch.
- Elasticsearch builds a search index.
- Searches run against Elasticsearch.

---

### Why Is It Needed?

Search engines such as:

* Google Search
* YouTube Search
* Amazon Search

receive millions of identical queries every day.

Examples:

```text id="0wk8mu"
iphone
macbook
cricket score
weather today
```

Executing these searches repeatedly from search cluster can be expensive and unnecessary.

---

### What Should Be Cached?

Popular and frequently repeated search queries:

* Trending searches
* Product searches
* News searches
* Video searches
* Frequently accessed filters and aggregations

For example:

```text id="c2q5h8"
search:iphone
search:macbook
search:cricket_score
```

The cached value contains the search results returned by Elasticsearch.

---

### Request Flow

```text id="6f8j1p"
Search Request
      |
 Check Cache
      |
Hit ----------> Return Results
      |
Miss
      |
Elasticsearch
      |
Store Results in Cache
      |
Return Results
```

### Cache Hit

```text id="wkzv3v"
User
  |
Cache
  |
Return Results
```

No search execution is needed.

### Cache Miss

```text id="8bb7e9"
User
  |
Cache (Miss)
  |
Elasticsearch
  |
Store in Cache
  |
Return Results
```

The search result is computed once and then cached for future requests.

---

### Benefits

### 1. Faster Search Responses

Results can be returned from memory rather than executing a search query.

### 2. Reduced Search Cluster Load

Fewer requests reach Elasticsearch, reducing CPU and memory usage.

### 3. Better Scalability

The search system can handle significantly more users because repeated queries are served from cache.

### 4. Lower Infrastructure Cost

Fewer Elasticsearch nodes are required when popular queries are cached.

---

## 7. Recommendation Cache

- A **Recommendation Cache** stores precomputed recommendations so that users can receive personalized suggestions instantly without repeatedly running expensive recommendation algorithms.

- It is placed between the user-facing application and the recommendation or machine learning service.

- Instead of generating recommendations on every request, the system retrieves them from the cache.
- A recommendation cache stores precomputed recommendations for a user, usually based on their interests, behavior, history, and preferences.

**Example: Netflix**

Suppose User 123 has watched:
```
Breaking Bad
Narcos
Ozark
```
The recommendation service analyzes this behavior and generates:
```
Better Call Saul
Mindhunter
Peaky Blinders
```
Instead of recomputing these recommendations every time the user opens Netflix, the results are cached:
```
recommendation:user123
    ->
[
  Better Call Saul,
  Mindhunter,
  Peaky Blinders
]
```

---

### Why Is It Needed?

Generating recommendations can be computationally expensive.

Typical recommendation generation times:

```text id="z3h5q1"
Simple Model      : 100 ms
Complex Model     : 500 ms
Large ML Pipeline : Several Seconds
```

Running these computations for every user request would create high latency and significant infrastructure costs.

---

### What Should Be Cached?

Precomputed recommendations for each user.

Examples:

```text id="a7m2c8"
recommendation:user123
recommendation:user456
recommendation:user789
```

Cached values may contain:

* Recommended videos
* Recommended products
* Recommended movies
* Recommended posts
* Personalized home feeds

---

### Example

For a user with ID 123:

```text id="v4q1n7"
recommendation:user123
```

Cached value:

```text id="f8r6w3"
[
  Movie A,
  Movie B,
  Movie C,
  Movie D
]
```

When the user opens the application, recommendations can be returned immediately from cache.

---

### Request Flow

```text id="n5j2p8"
User Request
      |
Check Recommendation Cache
      |
Hit ----------> Return Recommendations
      |
Miss
      |
ML Service
      |
Generate Recommendations
      |
Store in Cache
      |
Return Recommendations
```

---

## Cache Invalidation (Most Important Interview Topic)

### What is Cache Invalidation?

Caching improves performance by storing data closer to users, but it introduces a new challenge:

**What happens when the underlying data changes?**

This problem is called **cache invalidation**.

A cache is only useful if it contains accurate data.

---

### Example

Suppose a user updates their profile name.

Before update:

```text id="e4j7nk"
Database: Mike
Cache:    Mike
```

User changes name to John.

If only the database is updated:

```text id="c8m2qx"
Database: John
Cache:    Mike
```

The cache now contains stale data.

Future requests may continue to show:

```text id="g5r9wp"
Mike
```

even though the database contains:

```text id="u2x6zt"
John
```

---

### Correct Flow

When data changes:

```text id="n7v4ha"
Database Updated
      |
Invalidate Cache
      |
Next Read Reloads Cache
```

The next request fetches fresh data from the database and repopulates the cache.

---

### Common Cache Invalidation Strategies :
### Question : How do you keep cache and DB consistent?

### 1. TTL (Time-To-Live)

Each cache entry automatically expires after a fixed duration.

Example:

```text id="s3k8md"
user:100
TTL = 5 minutes
```

After 5 minutes:

```text id="r1f7yb"
Cache Entry Removed
```

The next request reloads data from the database.

### Benefits

* Very simple
* Easy to implement
* No manual invalidation required

### Drawback

Stale data may be served until expiration.

Example:

```text id="x9d4tw"
User updates profile
1 minute later
Cache still returns old value
```

until the TTL expires.

---

### 2. Write-Through Cache

Every write updates both the cache and the database.

```text id="v6q2cn"
Write
  |
Cache
  |
Database
```

Example:

```text id="t8k5wh"
Update user name
      |
Update Redis
      |
Update Database
```

### Benefits

* Cache and database stay synchronized
* Reads are fast

### Drawback

* Every write must update two systems

---

### 3. Cache-Aside Pattern (Most Common)

### Read Flow

```text id="z4p7rn"
Cache
  |
Miss
  |
Database
  |
Store in Cache
  |
Return
```

### Write Flow

```text id="b2m9kj"
Update Database
      |
Delete Cache Entry
```

Example:

```text id="w7n3vx"
UPDATE users
SET name = 'John'
WHERE id = 100;
```

Then:

```text id="f5q1de"
DELETE user:100
```

Next read:

```text id="g8t4ms"
Cache Miss
    |
Database
    |
Store Fresh Value
```

### Benefits

* Simple
* Efficient
* Widely used with Redis

This is the most common interview answer.

---

### 4. Write-Back Cache

Writes go to the cache first.

```text id="y3v8qa"
Write Cache
     |
Async Database Update
```

Example:

```text id="n6r4kc"
Update Redis
      |
Queue Update
      |
Database Updated Later
```

### Benefits

* Very fast writes
* Reduced database load

### Drawbacks

* Risk of data loss if cache fails before DB update
* More complex architecture

Used in high-performance systems where write speed is critical.

---

### Comparison
```
| Strategy      | Read Speed | Write Speed | Consistency | Complexity |
| ------------- | ---------- | ----------- | ----------- | ---------- |
| TTL           | Fast       | Fast        | Medium      | Low        |
| Write-Through | Fast       | Medium      | High        | Medium     |
| Cache-Aside   | Fast       | Medium      | High        | Low        |
| Write-Back    | Fast       | Very Fast   | Lower       | High       |
```
---

### Real-Life Analogy

Imagine a notice board showing today's meeting schedule.

### Without Invalidation

```text id="m8v2xp"
Office Schedule Updated
      |
Notice Board Not Updated
```

Employees continue seeing outdated information.

### With Invalidation

```text id="d4q7nh"
Schedule Updated
      |
Notice Board Updated
```

Everyone sees the latest information.

---

