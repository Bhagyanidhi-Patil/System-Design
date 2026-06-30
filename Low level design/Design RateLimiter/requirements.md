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

#### Step 1: Clarify Requirements

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

#### Step 2: Fixed Window Algorithm

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

#### Step 3: Required Classes
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