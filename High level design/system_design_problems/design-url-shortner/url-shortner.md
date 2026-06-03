# URL Shortener Design

## Problem Statement

Design a service that:

- Converts long URLs into short URLs.
- Redirects users from short URL to original URL.
- Supports billions of URLs.
- Handles high read traffic.
- Provides low latency redirects.

**Example**:

- Input: `https://www.example.com/blog/system-design/url-shortener`
- Output: `https://short.ly/aB3xYz`

## Functional Requirements

### Core Requirements

- User submits a long URL.
- System generates a unique short URL.
- User can access original URL using short URL.

### Additional Requirements

- Custom short URLs
  - `short.ly/myblog`
- URL expiration
- Click analytics
- User accounts

## Non-Functional Requirements

### Availability

Redirects should always work.

**Target:**

- `99.99%` availability

### Scalability

- Store billions of URLs.

### Low Latency

Redirect should happen within:

- `< 100 ms`

### Durability

- URLs should never be lost.

## Capacity Estimation

### Assume:

- Traffic: `100 million new URLs/day`
- Reads are much higher than writes.
- Read : Write = `100 : 1`

### Decimal Units Reference

Used for quick estimations in interviews.

| Unit | Bytes | Scientific notation |
| ---- | ----- | ------------------- |
| `1 KB` | `1,000` | `10³` |
| `1 MB` | `1,000,000` | `10⁶` |
| `1 GB` | `1,000,000,000` | `10⁹` |
| `1 TB` | `1,000,000,000,000` | `10¹²` |
| `1 PB` | `1,000,000,000,000,000` | `10¹⁵` |

### Indian Number System Reference

| Million (M) | Indian Number System |
| ----------- | -------------------- |
| `1M`         | `10 lakh`              |
| `5M`        | `50 lakh`              |
| `10M`         | `1 crore`              |
| `100M`        | `10 crore`             |
| `1000M (1B)`  | `100 crore`            |

### Storage

- Average URL length: `500 bytes`

When we say an average URL takes `500 bytes`, it means the stored data for one URL record is approximately `500 bytes` in memory/storage.

### Example

**URL:**

`https://www.example.com/blog/system-design/url-shortener`

This URL contains about `58` characters.

In UTF-8 encoding, standard English characters usually take `1 byte` each, so:

- `58 characters ≈ 58 bytes`

However, in a real URL shortener database record, you don't store only the URL string.

**Example record:**

```json
{
  "id": 123456789,
  "short_code": "aB3xYz",
  "original_url": "https://www.example.com/blog/system-design/url-shortener",
  "created_at": "2026-05-31T10:00:00Z",
  "user_id": 1001
}
```

| Field                      | Bytes              |
| -------------------------- | ------------------ |
| `id`                       | `8`                |
| `short_code`               | `6-10`             |
| `original_url`             | `50-200+`          |
| `timestamps`               | `8-16`             |
| `user_id`                  | `8`                |
| `DB metadata/index overhead` | `100-300+`       |
| **Total**                  | **~200-500 bytes** |

## Bit vs Byte

- `1 bit = 0 or 1`
- `8 bits = 1 byte`

**Example:**

- Character `'A'`
- ASCII value = `65`

Binary:

- `01000001`

- `= 8 bits`
- `= 1 byte`

The character `'A'` is typically `1 byte`, not `1 bit`.

**Character:**

- `A`

**ASCII value:**

- `65`

**Binary representation:**

- `01000001`

## Daily and Yearly Storage

If each URL record is `500 bytes` and you store:

- `100 million URLs/day`

Then:

- `100,000,000 × 500 bytes`
- `= 50,000,000,000 bytes`
- `≈ 50 GB/day`

**Per year:**

- `≈ 18 TB/year`

Need distributed storage.


## High-Level Design

```text
                    +----------------+
                    |     Users      |
                    +--------+-------+
                             |
                             v
                    +----------------+
                    | Load Balancer  |
                    +--------+-------+
                             |
             +---------------+---------------+
             |                               |
             v                               v

    +----------------+             +----------------+
    | URL Service    |             | Redirect       |
    | (Create URL)   |             | Service        |
    +-------+--------+             +-------+--------+
            |                              |
            |                              |
            v                              v
    +----------------+             +----------------+
    | ID Generator   |             | Redis Cache    |
    | (Base62)       |             +-------+--------+
    +-------+--------+                     |
            |                              |
            |                      Cache Miss?
            |                              |
            v                              v
    +---------------------------------------------+
    |             URL Database                    |
    |---------------------------------------------|
    | short_code  |  original_url                 |
    | abc123      |  https://google.com           |
    | xyz789      |  https://amazon.com           | 
    +---------------------------------------------+
                             |
                             |
                             v
                    +----------------+
                    | Kafka / Queue  |
                    +-------+--------+
                            |
                            v
                    +----------------+
                    | Analytics      |
                    | Click Count    |
                    | Geo Stats      |
                    +----------------+
```

## API Design

The first thing after requirements is defining the APIs.

We have two major operations:

### API 1: Create Short URL

**Request:**

- `POST /api/v1/shorten`

**Body:**

```json
{
  "url": "https://www.google.com/search?q=system+design"
}
```

**Response:**

```json
{
  "shortUrl": "https://short.ly/abc123"
}
```

**What happens internally?**

```text
Client
  |
  v
URL Service
  |
Validate URL
  |
Generate Short Code
  |
Store Mapping
  |
Return Short URL
```

**Example:**

- Long URL: `https://google.com/search?q=system+design`
- Generated code: `abc123`
- Stored: `abc123 -> https://google.com/search?q=system+design`

### API 2: Redirect URL

**Request:**

- `GET /abc123`

**User enters:**

- `https://short.ly/abc123`

**Server finds:**

```text
abc123
   |
   v
https://google.com/search?q=system+design
```

**Response:**

- `302 Redirect`
- `Location: https://google.com/search?q=system+design`

Browser automatically opens Google.

## Why 302 Redirect?

- `301`
    - Permanent redirect.
    - Browser caches forever.

- `302`

    - Temporary redirect.
    - Allows analytics tracking.
    - Most URL shorteners use 302.

**Reason:**

- Every click reaches your server.
- You can count clicks.

## Database Design

### Basic Table

`URL_MAPPING`

| Column       | Purpose           |
| ------------ | ----------------- |
| id           | Unique numeric ID |
| short_code   | abc123            |
| original_url | Actual URL        |
| created_at   | Creation time     |
| expiry_time  | Expiration        |
| user_id      | Owner             |

**Example rows:**

| id | short_code | original_url |
| -- | ---------- | ------------ |
| 1  | abc123     | google.com   |
| 2  | xyz999     | amazon.com   |

### Why Keep an ID?

- Because Base62 encoding needs a unique integer.

**Example:**

- `ID = 1000`

Convert:

- `1000 -> g8`

Store:

- `1000`
- `g8`
- `https://google.com`

## Short URL Generation

### Requirement

Generate:

- `short.ly/abc123`

Must be:

- Unique
- Small
- Fast
- Scalable

### Approach 1: Hashing

Suppose:

- `URL`
- `↓`
- `MD5`
- `↓`
- `7f138a09169b...`

Take first 7 chars.

- `7f138a0`

Short URL:

- `short.ly/7f138a0`

**Problem: Collision**

- Two URLs may generate same prefix.

Example:

- `URL1 -> 7f138a0`
- `URL2 -> 7f138a0`

Now both want same short code.

- Need collision handling.
- This adds complexity.

### Approach 2: Base62 Encoding

**Character Set**

- `a-z = 26`
- `A-Z = 26`
- `0-9 = 10`

Total = `62`

Hence Base62.

**Example**

- Database ID: `125`

Convert to Base62:

- `125 -> cb`

Short URL:

- `short.ly/cb`

### Why Base62?

- Decimal uses only 10 symbols: `0-9`.
- So the number `1000000000` takes 10 characters in decimal.
- Base62 uses 62 symbols: `0-9`, `a-z`, and `A-Z`.
- Using more symbols means the same number can be written in fewer characters.

**Example:**

- Decimal: `1000000000` → `10 characters`
- Base62: `15ftgG` → `6 characters`

So Base62 makes short codes shorter.

## Capacity Calculation

Suppose:

- `100 Million URLs/day`

Per Year:

- `100M × 365`

≈

- `36.5 Billion URLs`

### Base62 Capacity

Base62 means we have:

- `a-z = 26`
- `A-Z = 26`
- `0-9 = 10`

Total:

- `26 + 26 + 10 = 62`

possible characters for each position.

- Suppose code length is 1.
  - Number of possibilities: `62`
- Suppose code length is 2.
  - Number of possibilities: `62² = 3,844`

Length = `7`

Possible combinations:

- `62^7`

Let's calculate:

- `62^7 = 3,521,614,606,208`

≈

- `3.5 Trillion URLs`

Enough for years.

## Redirect Flow

Reads are much more frequent than writes.

**Example:**

- You create: `short.ly/abc123`
- This is 1 write.
- Now imagine you share it on social media and 1 million people click it.
- `1 write`
- `1,000,000 reads`

If every click goes to the database:

```text
User
  ↓
Database
```

the database gets overloaded.

So we add a cache (Redis).

### Without Cache

Suppose:

- `abc123 -> https://google.com`

stored in DB.

User clicks:

- `short.ly/abc123`

Flow:

```text
User
 ↓
Redirect Service
 ↓
Database
 ↓
Get URL
 ↓
302 Redirect
```

Every click hits DB.

### With Cache

Store in Redis:

- Key: `abc123`
- Value: `https://google.com`

Now flow becomes:

```text
User
 ↓
Redirect Service
 ↓
Redis
Cache Hit
```

Redis finds:

- `abc123 -> google.com`

Immediately return:

- `302 Redirect`

No DB call.

### Cache Miss

Redis doesn't have it.

```text
User
 ↓
Redirect Service
 ↓
Redis (miss)
 ↓
Database
 ↓
Get URL
 ↓
Save in Redis
 ↓
Return Redirect
```

Future requests become cache hits.

This is the essence of Point 10.

### Why Redis?

- Redis stores data in RAM.

RAM access:

- Microseconds

Database:

- Milliseconds

Much faster.

## Database Choice

### SQL or NoSQL?

#### SQL Option

Examples:

- MySQL
- PostgreSQL

Advantages:

- ACID
- Strong consistency
- Easy indexing
- Simple queries

Schema:

- `id`
- `short_code`
- `original_url`

Perfect for URL mappings.

#### NoSQL Option

Examples:

- Apache Cassandra
- MongoDB

Advantages:

- Horizontal scaling
- Huge write throughput

Useful at very large scale.

> "Initially I would use PostgreSQL because the data model is simple and consistency is important. Once the scale reaches billions of URLs, I would shard the database or move to Cassandra for better horizontal scalability."

### SQL vs NoSQL

| Feature        | SQL                     | NoSQL                                      |
| -------------- | ----------------------- | ------------------------------------------ |
| Data Structure | Tables (rows & columns) | Documents, key-value, column-family, graph |
| Schema         | Fixed schema            | Flexible schema                            |
| Scaling        | Mostly vertical         | Mostly horizontal                          |
| Joins          | Supported               | Usually limited/no joins                   |
| Consistency    | Strong (ACID)           | Often eventual consistency                 |
| Examples       | PostgreSQL, MySQL       | MongoDB, Apache Cassandra                  |

### SQL Example

Suppose you're building an e-commerce app.

**Users Table**

| user_id | name  |
| ------- | ----- |
| 1       | John  |
| 2       | Alice |

**Orders Table**

| order_id | user_id |
| -------- | ------- |
| 101      | 1       |
| 102      | 2       |

Relationship:

- `User -----> Orders`

### NoSQL Example

In MongoDB, one record might look like:

```json
{
  "userId": 1,
  "name": "John",
  "orders": [
    {
      "orderId": 101
    },
    {
      "orderId": 102
    }
  ]
}
```

## Why NoSQL is Preferred Over SQL

### 1. Simple Key-Value Access Pattern

URL shortener is essentially a key-value lookup:

```
key -> value
```

**Example:**
```
abc123 -> https://example.com/long-url
```

This pattern naturally fits NoSQL stores like:
- **Redis**
- **DynamoDB**
- **Cassandra**

**Query:**
```
GET abc123

Returns: https://example.com/long-url
```

✅ Very fast and simple.

### 2. Massive Read Traffic

**Scenario:**
- 100 million URLs stored
- 1 billion redirects/day

**Every redirect requires:**
- Short code → Long URL lookup

**Why NoSQL excels:**
- Key-value stores are optimized for this pattern
- Lookup complexity: **O(1)**
- Perfect for read-heavy workloads

### 3. Easier Horizontal Scaling

**SQL Scaling (Traditional):**

```
    Primary
   /       \
Replica1   Replica2
```

- Writes go to primary
- Eventually primary becomes bottleneck

**NoSQL Scaling (Distributed):**

```
      Hash
       |
 +-----+-----+
 |     |     |
Sh1   Sh2   Sh3
```

**Sharding by short code:**
- `abc123` → Shard1
- `xyz999` → Shard2

✅ Adding more shards is usually straightforward

### 4. High Write Throughput

**Scenario:**
- 10 million new URLs/day
- Each URL generates: `INSERT` operation

Distributed NoSQL databases are designed for:
- Huge write volumes
- Parallel writes across shards

**Examples:**
- Apache Cassandra
- Amazon DynamoDB

### 5. Joins Are Not Needed

**SQL Use Case (Complex):**
```sql
SELECT *
FROM users
JOIN orders
JOIN payments
JOIN products
```

This requires:
- Multiple tables
- Relational integrity
- Complex transactions

**URL Shortener Use Case (Simple):**
```
ShortCode → LongURL
```

✅ No joins needed
✅ No relational modeling needed
✅ Simple direct lookups

### Why Companies Still Use SQL Sometimes

> "A URL shortener does not require NoSQL. SQL can work very well, especially in early stages. The choice depends on scale."

**SQL Advantages in early stages:**
- Simpler operational complexity
- Easier schema modifications
- ACID guarantees
- Strong consistency

**Typical progression:**
1. Start with PostgreSQL (simple, reliable)
2. As scale increases, shard the database
3. At billions of URLs, migrate to Cassandra for better horizontal scalability

## Horizontal Scaling Deep Dive

### What is Horizontal Scaling?

**Vertical Scaling (Making one server bigger):**

```
DB Server
   |
1000 QPS (limited by single machine)
```

❌ Problems:
- Single machine has limits
- More expensive
- Bottleneck at max capacity

**Horizontal Scaling (Adding more servers):**

```
+------+
| DB1  | 1000 QPS
+------+

+------+
| DB2  | 1000 QPS
+------+

+------+
| DB3  | 1000 QPS
+------+

Total: 3000 QPS
```

✅ Advantages:
- Linear scalability
- Cost-effective
- No single bottleneck

### Why is it Easy in NoSQL?

**Scenario: URL Data**

```
abc123 → google.com
xyz456 → amazon.com
fgh789 → facebook.com
```

**Distribution across shards:**

```
DB1: abc123 → google.com
DB2: xyz456 → amazon.com
DB3: fgh789 → facebook.com
```

**Routing Logic:**

```
shortCode = "abc123"
ShardId = hash(shortCode) % N

Query Shard1 for abc123
```

**Key Characteristics:**

✅ No relationship exists between records
✅ Each record is independent
✅ Very easy to partition

**Internal Note:**

📌 Many NoSQL databases store data in structures that look like tables internally (e.g., Apache Cassandra uses rows and columns).

**Example NoSQL Record Structure:**

```
Shard1 → user123 → {...}
Shard2 → user456 → {...}
Shard3 → user789 → {...}
```

**Lookup behavior:**
- A request typically touches only **one shard**
- No distributed joins needed
- Fast, localized queries

### Why is SQL Harder?

**SQL provides strong guarantees:**

SQL databases support:
- ✅ ACID transactions
- ✅ JOINs across tables
- ✅ Foreign keys
- ✅ Strong consistency

These features create **dependencies between data**.

**Example: SQL Relational Model**

```
Users Table
┌─────────┐
│ user_id │
├─────────┤
│    1    │
│    2    │
└─────────┘

Orders Table
┌─────────┬─────────┐
│order_id │ user_id │
├─────────┼─────────┤
│   101   │    1    │
│   102   │    2    │
└─────────┴─────────┘
```

**Dependency:** Orders depend on Users

**When scaling across machines:**

```
Machine1 → Users Table
Machine2 → Orders Table
```

Challenges:
- ❌ Database must coordinate between machines
- ❌ Distributed transactions are complex
- ❌ Maintaining consistency is harder
- ❌ Scaling becomes significantly more difficult

**Conclusion:**
SQL's relational guarantees are powerful but make horizontal scaling complex. NoSQL's simpler model (key-value, no joins) makes sharding and scaling much more straightforward.
