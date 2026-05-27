# Rate Limiting

Rate limiting is a mechanism used in system design to control how many requests a client can make to a service within a given time window.

It protects systems from:
```
- Abuse (spam, brute force, scraping)
- Traffic spikes
- Resource exhaustion
- Noisy neighbors in multi-tenant systems
- Cascading failures
```

## Examples
```
“100 API requests per minute per user”
“5 login attempts per 10 minutes”
“10 messages/sec per IP”
```

## Why Rate Limiting Matters

### Without rate limiting
```
- One user can overwhelm servers
- Databases can get overloaded
- APIs become unavailable
- Costs increase dramatically
```
### With rate limiting
```
- Traffic becomes predictable
- Fair usage is enforced
- Systems stay stable under load
```
## Core Concepts

### 1. Key

What are you limiting?

Common keys:

- User ID
- IP address
- API key
- Session ID
- Tenant ID

Example:
```
user_123 → 100 req/min
```
### 2. Limit

Maximum allowed requests.

Example:

```
100 requests/minute
```

### 3. Time Window

Period during which requests are counted.

Examples:
```
- Per second
- Per minute
- Per hour
```

## Common Rate Limiting Algorithms

### 1. Fixed Window Counter

The simplest algorithm.

Example:
```
- Limit = 100 requests/minute
```
You maintain:
```
- counter
- current window
```
How it works:
```
- Count requests in current minute
- Reset counter every minute
```
Example:
```
12:00 → counter = 0
User sends 100 requests → allowed
101st request → blocked

12:01 → counter resets
```
**Pros**

- Very simple
- Fast
- Low memory

**Cons**

- Boundary problem

Example:
```
12:00:59 → 100 requests
12:01:00 → another 100 requests
```
Effectively:
```
200 requests in 2 seconds
```

This is called the boundary problem.

---

### 2. Sliding Window Log

More accurate.

Store timestamp of every request.

How it works:
```
- Remove timestamps older than the window
- Count remaining requests
- Allow or reject
```
Example:
```
Limit = 3 req/min

Requests:
10:00:10
10:00:20
10:00:40

4th request at 10:00:50 → blocked
```

**Pros**

- Accurate
- No boundary issue

**Cons**

- High memory usage
- Expensive for large scale

Because storing every request timestamp is costly.

---

### 3. Sliding Window Counter

Goal:

Avoid the boundary problem of fixed window while using less memory than sliding logs.

Idea:

Instead of storing every request timestamp, divide time into small buckets.

Example:
```
1 minute window
→ split into 6 buckets
→ each bucket = 10 seconds
```

We count requests inside each bucket.

#### Simple Flow

```
Time Window = 60 sec

|----|----|----|----|----|----|
0   10   20   30   40   50   60

Buckets:
[5] [8] [3] [6] [2] [1]
```
Suppose:
```
- Limit = 10 requests/minute
- Bucket size = 10 seconds
```
So 1 minute becomes:
```
| B1 | B2 | B3 | B4 | B5 | B6 |
```
Each bucket stores how many requests came during that 10-sec interval.

#### Example Timeline
```
- 00-10 sec -> 3 requests
- 10-20 sec -> 2 requests
- 20-30 sec -> 1 request

Stored as:

| 3 | 2 | 1 | 0 | 0 | 0 |
```
Now a new request arrives at 32 sec.

We calculate:
```
3 + 2 + 1 = 6 requests in last 60 sec

If limit is 10:

- 6 < 10 ? Allow request

Then current bucket becomes:

| 3 | 2 | 1 | 1 | 0 | 0 |
```
**Pros**
```
-  More accurate
-  Less memory than sliding logs
-  Good for distributed systems
```
**Cons**
```
-  Slight approximation errors
-  More complex than fixed window
```

---

### 4. Token Bucket (VERY IMPORTANT)

This is one of the most commonly used algorithms.

#### Core Idea

Imagine a bucket containing tokens.
```
- Each request needs 1 token
- Tokens refill continuously
        +----------------+
        |  Token Bucket  |
        |                |
        |  ● ● ● ● ●     |
        +----------------+
              ^
              |
              |
         Incoming Request

If a token exists:

- Consume token ? Allow request

If no token exists:

- Reject request
```

#### Example

Configuration:
```
- Bucket size = 5
- Refill rate = 1 token/sec
```

Initial state:

`[● ● ● ● ●]`

User sends 5 requests immediately:
```
Req1 → allow
Req2 → allow
Req3 → allow
Req4 → allow
Req5 → allow
```

Bucket now:

`[empty]`

6th request:
```
- Rejected
```
After 1 second:
```
- 1 token added
- Now 1 request allowed again
```

#### Visual Timeline

```
Time → →

Tokens:
5 → 4 → 3 → 2 → 1 → 0

Requests:
✓   ✓   ✓   ✓   ✓   ✗

(after refill)

0 → 1
      ✓
```

#### Why Token Bucket is Popular

It allows:

- controlled bursts
- stable average traffic

Example:

- Normal limit: 10 req/sec
- But allow burst: 50 requests instantly

Very useful in real systems.

**Pros**

- Supports bursts
- Smooth traffic
- Efficient
- Easy with Redis

**Cons**

- Slightly harder implementation

---

### 5. Leaky Bucket

This focuses on making outgoing traffic smooth.

#### Core Idea

Requests enter the bucket quickly, but leave at a constant speed — like water leaking slowly.

#### Flow Diagram

```text
Incoming Requests
   vvvvvvvvvv

 +----------------+
 |                |
 |  Leaky Bucket  |
 |                |
 +----------------+
         |
         | constant speed
         v

  Processed Requests
```

#### Example

Incoming:
```
- 100 requests instantly
```
System processes:
```
- 10 requests/sec
```
So requests are queued.

#### Visual

```
Incoming:
🔥🔥🔥🔥🔥🔥🔥🔥🔥🔥

Outgoing:
✓   ✓   ✓   ✓
(steady speed)
```

#### What Happens if Bucket Full?

If too many requests arrive:
```
- Bucket full -> Drop new requests
```

---

### Token bucket allows bursts while controlling the average rate over time.

#### Example
```
Bucket size = 5
Refill rate = 5 tokens/sec
```
Meaning:
```
1 token every 200 ms
Real Timeline
```
Initially:
```
[● ● ● ● ●]
```
At 0.59 sec:
User consumes all 5.

Bucket:

`[empty]`
What Happens Next?

At:
```
0.79 sec → +1 token
0.99 sec → +1 token
1.19 sec → +1 token
```
...

Tokens refill gradually.

NOT:
```
+5 suddenly at 1 sec
```
So At 1.01 sec

Only about:
```
2 tokens available
NOT 5
```

So user can only make:
```
2 requests
```

---

### Fixed Window Problem

Suppose:

Limit = 5 req/sec

Window:
```
0-1 sec
1-2 sec
2-3 sec
```
What Happens

User sends:
```
0.99 sec → 5 requests
1.01 sec → 5 requests
```
Total:
```
10 requests in 0.02 sec
```
This happens because:

Counter resets completely at boundary

This creates a sharp discontinuity.
