# Problem Statement

Design a Rate Limiter

A rate limiter controls how many requests a user/client can make within a specific time period.

Examples:
- A user can send 100 API requests per minute
- A client can make 1000 requests per hour
- Login attempts limited to 5 per minute

If the limit is exceeded:
- Reject request (429 Too Many Requests)
- Delay request
- Queue request

## Why Do We Need Rate Limiting?

### Protect Servers

Prevent overload from excessive traffic.

### Prevent Abuse

Stop bots, brute-force attacks, API misuse.

### Fair Resource Usage

Prevent one user from consuming all resources.

### Cost Control

Reduce infrastructure costs, Reduces unnecessary backend work by rejecting excess traffic early.

Example:

Without rate limiting:

User A → 1,000,000 requests
Server crashes

With rate limiting:

Allowed = 100 requests/minute
Excess requests rejected

## Functional Requirements

### Must Have
- Allow requests within limit
- Reject requests beyond limit
- Support different users
- Support configurable limits

Example:

User A → 100 req/min
User B → 1000 req/min

### Nice to Have
- Distributed deployment
- Low latency
- High availability
- Analytics

## Non-Functional Requirements

### High Performance

Rate limit check should be very fast.

Target:

< 1 ms

### Scalable

Millions of users.

### Fault Tolerant

Failure of one node shouldn't disable rate limiting.

### Consistent

Avoid allowing excess requests.

## High-Level Design

```
                +------------+
Client -------> | API Gateway |
                +------------+
                       |
                       v
                +------------+
                | RateLimiter|
                +------------+
                       |
           Allowed?    |
          Yes / No     |
             |         |
             v         v
      Backend API   Reject 429
```

Flow:
- Request arrives
- Rate limiter checks count
- If under limit → allow
- Else → reject

## Key Design Decision

Where should Rate Limiter be placed?

### Option 1: Client-Side Rate Limiter

```
Client App
    |
Rate Limiter
    |
Server
```

How it works

The rate-limiting logic runs inside the mobile app, browser, or SDK.
The rate limiter runs inside the client application itself.

Example:

```js
if (requestsInLastMinute > 100) {
    blockRequest();
}
```

Advantages
- ✅ Reduces accidental excessive requests
- ✅ Saves network traffic
- ✅ Improves user experience by preventing obvious spam

Disadvantages
- ❌ User controls the client
- ❌ Can be bypassed easily
- ❌ Attackers can modify the code or use tools like Postman/cURL
- ❌ Not suitable for security enforcement

### Option 2: Server-Side Rate Limiter

```
Client
   |
   v
Server
   |
Rate Limiter Check
```

or logically:

```
Request
   |
Server receives request
   |
Check limit
   |
Allow / Reject
```

How it works

Every request reaches the server first.

The server checks:
- IP address
- User ID
- API key
- Session

before processing.

Advantages
- ✅ Cannot be bypassed
- ✅ Full control
- ✅ Works for all clients
- ✅ Accurate tracking

Disadvantages
- ❌ Every request still reaches your server
- ❌ During attacks, the server spends resources handling requests
- ❌ Can become expensive at large scale

Example

100,000 requests arrive

Server receives all 100,000
        |
Rate limiter blocks 99,000

The protection works, but your server still handled those requests.

### Option 3: API Gateway Rate Limiter (Industry Standard)

```
Client
   |
   v
API Gateway
   |
Rate Limiter
   |
Backend Services
```

How it works

The gateway sits in front of all backend services.

Requests are checked before reaching your application.

```
Request
   |
Gateway
   |
Rate Limit Check
   |
Allowed? ---- No --> 429
   |
  Yes
   |
Backend
```

Advantages
- ✅ Stops abuse before backend is touched
- ✅ Protects all services centrally
- ✅ Easier to manage
- ✅ Highly scalable
- ✅ Works across multiple microservices

Disadvantages
- ❌ Extra infrastructure
- ❌ Gateway can become a bottleneck if poorly designed
- ❌ More operational complexity

Example

1,000,000 requests

Gateway receives them
      |
Blocks 950,000
      |
Only 50,000 reach backend

## What Large Companies Usually Do

Companies such as Netflix, Uber, and Stripe typically use multiple layers:

```
Client
   |
Client-side limiter
   |
CDN / Edge
   |
API Gateway Rate Limiter
   |
Backend Rate Limiter
   |
Service
```

This is called defense in depth.

## Core Algorithms

### A. Fixed Window Counter

Example:
- Limit = 100 req/min
- Window: 12:00 - 12:01
- Store: User A = 97
- Request arrives: Count++
- At minute boundary: Counter reset

Example:
- 12:00:50 -> 100 requests
- 12:00:59 -> limit reached
- At 12:01:00 counter resets.
- User can send 100 more requests
- Result: 200 requests in 10 seconds

Problem: burst traffic.

Complexity: O(1)

Very simple.

### B. Sliding Window Log

Store every request timestamp.

Example:
- 10
- 20
- 25
- 30
- 45

Request at 70 sec:
- Remove timestamps older than 70 - 60 = 10
- Keep: 20, 25, 30, 45
- Count timestamps.

If count exceeds limit → reject.

Advantages:
- Accurate.

Disadvantages:
- High memory usage.
- Millions of timestamps.

### C. Sliding Window Counter

Hybrid approach.

Store counters for small buckets.

Example:
- Minute divided into 10 sec buckets
- Bucket1 = 20
- Bucket2 = 30
- Bucket3 = 15

Estimate current rate.

Pros:
- Less memory
- Fairly accurate

Cons:
- Slight approximation

### D. Token Bucket (Most Popular)

Used in many production systems.

Idea:
- Bucket contains tokens.
- Capacity = 100
- Refill = 10/sec
- Request consumes token.

Example:
- Tokens = 100
- Request: Consume 1 token
- Now: 99
- After 1 sec: 109
- Cap at: 100

Pros:
- Allows short bursts.
- 100 requests instantly then controlled.

Complexity: O(1)

Excellent choice.

### E. Leaky Bucket

Imagine bucket with hole.

Incoming:
- 100 requests/sec

Outgoing:
- 10 requests/sec

Overflow requests dropped.

Pros:
- Smooth traffic.

Cons:
- Less burst-friendly.

## Database Design

For distributed systems we typically use Redis.

Why Redis?

Redis is:
- In-memory (very fast)
- Supports millions of operations/sec
- Atomic operations
- TTL support
- Easy clustering

A rate limit check usually becomes:

INCR userA

Redis automatically increments the value.

### Race Condition Problem

**Scenario:**
- Current count: **99**
- Limit: **100**
- Two requests arrive simultaneously

**Without Atomicity:**

| Operation | RL1 | RL2 |
|-----------|-----|-----|
| Read | 99 | 99 |
| Write | 100 | 100 |

**Result:**
- Both requests are allowed
- Actual total: **101 requests happened**
- ❌ Limit violated!

### How Redis Solves It

Redis executes commands **sequentially** (atomic operations).

**Command:** `INCR userA`

- **Request 1:** 99 → 100 ✓ (allowed)
- **Request 2:** 100 → 101 ✓ (rejected, exceeds limit)

Result:
- RL2 sees: `101 > 100`
- Request rejected
- ✅ Only one request crosses the boundary

### Is Redis a Bottleneck?

**Single Redis becomes a bottleneck:**

```
RL1
RL2
RL3
RL4
  |
Redis (overloaded)
```

⚠️ Problems:
- Eventually Redis becomes overloaded
- Single point of failure
- Scaling bottleneck

**Solution: Redis Cluster**

```
          Redis Cluster
    +---------+---------+
    |                   |
  Shard1             Shard2
    |                   |
  user1,user2       user3,user4
```

**Sharding Strategy:**
- Use: `hash(userId) % N`
- Example:
  - `user123` → Shard1
  - `user456` → Shard2
- ✅ Load gets distributed

## Distributed Rate Limiter

### Single Rate Limiter Server

Suppose we have only one rate limiter server.

```
Client
   |
   v
Rate Limiter
   |
Backend
```

Limit:

User A → 100 requests/minute

The rate limiter maintains:

UserA = 75

Every request goes through the same server, so counting is easy.

### Why Doesn't This Scale?

**Scenario:**
- 10 million users
- 100,000 requests/sec
- One rate limiter server cannot handle all traffic

**Multiple Rate Limiters with Local Counters:**

```
        Load Balancer
             |
      +------+------+
      |             |
     RL1           RL2
```

**Problem: Inconsistent Counters**

User A sends 110 requests:
- 55 requests → RL1
- 55 requests → RL2

| Server | Local Count | Check | Decision |
|--------|-------------|-------|----------|
| RL1 | 55 | 55 < 100 | ✓ Allow |
| RL2 | 55 | 55 < 100 | ✓ Allow |

**Actual Result:**
- Total requests: 55 + 55 = **110**
- Limit: **100**
- ❌ **Rate limit is broken!**

### Centralized Redis Solution

**Architecture: Single Source of Truth**

```
        Load Balancer
             |
      +------+------+
      |             |
     RL1           RL2
      \             /
       \           /
        +---+---+
        |  Redis |
        +-------+
```

**Request Flow:**

**Request 1 hits RL1:**
- UserA current count in Redis: `90`
- RL1 increments: `90 → 91`
- Check: `91 < 100` ✓ Allow

**Request 2 hits RL2:**
- UserA current count in Redis: `91`
- RL2 increments: `91 → 92`
- Check: `92 < 100` ✓ Allow

**Key Advantage:**
- ✅ RL1 and RL2 share the **same count**
- ✅ Single source of truth (Redis)
- ✅ Consistent regardless of which server receives the request

## API Design

### Successful Request

**Request:**
```
GET /api/orders
```

**Response:**
```json
{
  "allowed": true
}
```

### Rate Limit Exceeded

**Response Status:** `429 Too Many Requests`

```json
{
  "error": "Rate limit exceeded"
}
```

**Response Headers:**
```
X-RateLimit-Limit: 100
X-RateLimit-Remaining: 12
Retry-After: 45
```

## Scaling to Millions of Users

**Architecture:**

```
    Load Balancer
        |
  Rate Limiter Nodes
        |
   Redis Cluster
  +---+---+---+
  |   |   |   |
 Sh1 Sh2 Sh3 Sh4
```

**Sharding Strategy:**
- `user1` → Shard1
- `user2` → Shard2
- Sharding by: `hash(userId) % N`

## Handling Redis Failure

**Problem:** Redis goes down

**Options:**

### Fail Open
- **Strategy:** Allow traffic
- **Trade-off:**
  - ✅ Availability: **HIGH**
  - ❌ Security: **LOW**

### Fail Closed
- **Strategy:** Block traffic
- **Trade-off:**
  - ❌ Availability: **LOW**
  - ✅ Security: **HIGH**

---

### Flow chart for rate limiter 
```
                    Clients
                       |
                       v
                 Load Balancer
                       |
        +--------------+--------------+
        |                             |
        v                             v
    Gateway 1                    Gateway 2
        |                             |
        v                             v
     RL-1                          RL-2
        |                             |
        +------------+----------------+
                     |
                     v
              Redis Cluster
          +--------+--------+
          |        |        |
          v        v        v
      Shard1   Shard2   Shard3

                     |
                     v
                Backend APIs
```

### Why Multiple Rate Limiter Instances?

**Scenario:**
- Traffic: **1 million requests/sec**
- One rate limiter cannot handle all requests

**Solution: Scale Horizontally**

```
RL1
RL2
RL3
RL4
...
```

**Key Design:**
- Each instance is **stateless** (no local counters)
- All instances store counters in **Redis**
- Load balanced distribution of requests

### Why Multiple Redis Shards?

**Scenario:**
- 100 million users
- All counters in one Redis server

**Single Redis Becomes:**
```
user1 → Redis
user2 → Redis
user3 → Redis
   .
   .
   .
```

**Bottlenecks:**
- ❌ CPU bottleneck
- ❌ Memory bottleneck
- ❌ Network bottleneck

**Solution: Redis Cluster (Sharding)**

```
Shard1  Shard2  Shard3  Shard4
```

✅ Load distributed across shards

### How Does a Rate Limiter Find the Correct Shard?

Using consistent hashing or Redis Cluster routing.

**Example:**

**user123**
- Hash: `hash(user123) % 4 = 2`
- Store in: **Shard2**

**user456**
- Hash: `hash(user456) % 4 = 1`
- Store in: **Shard1**
