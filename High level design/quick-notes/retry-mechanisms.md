# Retry Mechanisms

Retry mechanism means:

If a request fails, the client or system automatically tries again to complete it.

This is very important in distributed systems where failures are normal (network, server crash, timeout, etc.).

## Why retries exist

In distributed systems, failures are common and often temporary:

- Network glitches
- Timeouts
- Service overload (HTTP 503)
- Brief dependency downtime

Instead of failing immediately, a system can retry the request hoping the issue resolves quickly.

## Basic idea

A retry mechanism:

- Detects a failure (timeout, error code, exception)
- Waits for some time (optional)
- Re-sends the request
- Stops after a limit (max retries / timeout / deadline)

## Common retry strategies

### 1. Fixed retry

Retry after a constant interval.

Example:

- Retry every 2 seconds, up to 3 times

Problem: can overload already struggling systems.

### 2. Exponential backoff (most important)

Wait time increases exponentially after each failure.

Example:

- 1st retry: 1s
- 2nd retry: 2s
- 3rd retry: 4s
- 4th retry: 8s

Often combined with:

- Jitter (randomness) to avoid synchronized retries

👉 This is widely expected in interviews.

### 3. Exponential backoff with jitter

Adds randomness:

Instead of:

- 4 seconds exactly

Use:

- 3.2s or 4.7s randomly

Why?

- Prevents “thundering herd problem” where many clients retry at the same time.

### 4. Linear backoff

Increase delay linearly:

- 1s → 2s → 3s → 4s

Less common, less efficient than exponential.

### 5. Immediate retry (rare)

Retry instantly once or twice for very short glitches.

Used only for ultra-low latency systems.

## Key design considerations

### 1. Idempotency (VERY IMPORTANT)

Retries must be safe.

If you retry:

- “Charge payment $100”

You must avoid charging twice.

So APIs should be:

- Idempotent using request IDs / deduplication keys

### 2. Retry limits

You must define:

- Max retries (e.g., 3–5)
- Max time window (e.g., 10 seconds total)

Otherwise:

- You risk infinite loops

### 3. Timeout vs retry

Retry only after:

- A request timeout occurs

But timeout must be:

- Carefully tuned (not too small or too large)

### 4. Retry only on transient errors

Retry:

- 500, 502, 503
- timeouts
- network errors

Do NOT retry:

- 400 Bad Request
- 401 Unauthorized
- 404 Not Found

### 5. Circuit breaker integration

Retries alone can worsen outages.

So combine with:

- Circuit breaker pattern

If a service is failing too much:

- stop retrying temporarily
- fail fast instead

### 6. Bulkhead isolation

Limit retry impact using:

- separate thread pools / queues
- request isolation per service

“What happens if payment service is down?”

Good answer flow:

- Client sends request → payment service fails (timeout)
- Retry with exponential backoff + jitter
- Ensure idempotency using transaction ID
- After max retries → send to fallback / queue (e.g., Kafka)
- Circuit breaker may open if failure rate increases



