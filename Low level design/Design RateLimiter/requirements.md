### Problem Statement

Suppose your API allows

- 5 requests per minute per user.

If a user sends
```
Request 1 ✔
Request 2 ✔
Request 3 ✔
Request 4 ✔
Request 5 ✔
Request 6 ❌
```
The sixth request should be rejected.

After one minute,
```
Request 7 ✔
```
should be allowed again.

---

### Clarify Requirements

During an interview, always begin with questions like:

- What is the rate limit?
> A Rate Limiter is a mechanism that controls the number of requests or operations allowed within a specified time period. If the limit is exceeded, additional requests are delayed, rejected, or ignored until the time window resets.
- Is it per user, IP, or API key?
- Which algorithm should we use?
- Should it be thread-safe?
- Is this an in-memory implementation or distributed?
- What should happen after the limit is exceeded?

Assume:

- Per user
- Fixed Window algorithm
- In-memory
- Thread-safe

---

#### Fixed Window Algorithm

Suppose
```
Limit = 5 requests
Window = 60 seconds

Timeline:

0 ----------------------60----------------------120

Window 1               Window 2
```
User requests:
```
10s ✔

15s ✔

22s ✔

30s ✔

45s ✔

50s ❌

65s ✔
```
At 60 seconds, the counter resets.

---

#### Required Classes
```
               +--------------------+
               | RateLimiter        |
               +--------------------+
               | allowRequest()     |
               +--------------------+
                         ▲
                         |
          +-------------------------------+
          | FixedWindowRateLimiter        |
          +-------------------------------+
          | users                         |
          | mutex                         |
          +-------------------------------+
                         |
                         |
            unordered_map<UserID, UserInfo>
                         |
               +------------------+
               | UserInfo         |
               +------------------+
               | requestCount     |
               | windowStartTime  |
               +------------------+
```

---

`The Fixed Window algorithm itself has nothing to do with users`. The algorithm is simply:

**Allow at most N requests within a fixed time window.**

The entity being limited depends on the requirement.

#### For example:

`Case 1: Per User (what we implemented)`
```
Alice → 5 requests/minute
Bob   → 5 requests/minute
Charlie → 5 requests/minute
```
Here we need:

unordered_map<string, UserInfo> users;

because each user has an independent counter.

`Case 2: Global Rate Limiter`

**Requirement:**
```
Allow only 100 requests per second for the entire application.

No user information is needed.
```
**You simply store:**
```
class FixedWindowRateLimiter
{
private:
    int requestCount;
    chrono::steady_clock::time_point windowStart;

    int maxRequests;
    chrono::seconds windowSize;
};
```
No map at all.

**Flow:**
```
Application
      |
      v
FixedWindowRateLimiter
      |
requestCount = 57
windowStart = 10:00:00
```
Every request updates the same counter.

`Case 3: Per API Key`
unordered_map<string, UserInfo> apiKeys;

where the **key** is an **API key.**

`Case 4: Per IP Address`
unordered_map<string, UserInfo> ipAddresses;

where the **key** is the **client's IP.**

---
#### Sliding Window Log
##### Idea

Instead of storing only
```
requestCount
windowStart
```
**we store the timestamp of every request.**

Example `(Limit = 5 requests / 60 seconds)`

Current time = 100 sec

Stored timestamps:
```
40
45
50
55
70
90
```
Before processing a new request, remove all timestamps older than 60 seconds.

Current Time = 100

Window = [40,100]
```
40
45
50
55
70
90
```
At time = 110

Window becomes
```
50
55
70
90
```
because
```
110 - 40 = 70 seconds
110 - 45 = 65 seconds
```
Those requests are outside the current window.

---

#### Class Diagram
```
RateLimiter
      ▲
      |
SlidingWindowRateLimiter
      |
      |
unordered_map<
    string,
    queue<timestamp>
    >
```

---
