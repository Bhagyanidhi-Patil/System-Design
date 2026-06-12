# Circuit Breaker Pattern

The Circuit Breaker Pattern is a design pattern used in microservices to **prevent a failing service from causing failures throughout the entire system.**

It is inspired by the electrical circuit breaker in a house.

## Real-Life Analogy

Imagine a house electrical circuit.

When too much current flows:

```text
Power Surge
     ↓
Circuit Breaker Trips
     ↓
Electricity Stops
```

The breaker disconnects the circuit to prevent damage.

Similarly, in software:

```text
Order Service
      ↓
Payment Service (Down)
```

Instead of continuously sending requests to a failing service, the circuit breaker stops the calls temporarily.

## Problem Without Circuit Breaker

Suppose:

```text
Order Service
      ↓
Payment Service
```

Payment Service crashes.

But Order Service keeps trying:

```text
Request 1 → Timeout
Request 2 → Timeout
Request 3 → Timeout
Request 4 → Timeout
...
```

### Problems:

* Threads get blocked
* CPU usage increases
* Response time increases
* Entire application may become slow

This is called a cascading failure.

## Solution: Circuit Breaker

A Circuit Breaker sits between services.

```text
Order Service
      ↓
Circuit Breaker
      ↓
Payment Service
```

When Payment Service starts failing repeatedly:

```text
Failure
Failure
Failure
Failure
```

Circuit Breaker opens the circuit.

```text
Order Service
      ↓
Circuit Breaker (OPEN)
      ✖
Payment Service
```

Now requests fail immediately instead of waiting for timeouts.

# Three States of Circuit Breaker

## 1. Closed State (Normal)

Everything is healthy.

```text
Order Service
      ↓
Circuit Breaker (CLOSED)
      ↓
Payment Service
```

All requests pass through.

### Example:

```text
Request → Success
Request → Success
Request → Success
```

## 2. Open State (Failure)

Too many failures occur.

```text
Request → Failure
Request → Failure
Request → Failure
```

Threshold exceeded.

Circuit opens:

```text
Order Service
      ↓
Circuit Breaker (OPEN)
      ✖
Payment Service
```

Now:

```text
New Request
      ↓
Immediate Failure
```

No call reaches Payment Service.

## 3. Half-Open State (Testing)

After some time:

```text
Wait 30 seconds
```

Circuit Breaker allows a few test requests.

```text
Order Service
      ↓
Circuit Breaker (HALF-OPEN)
      ↓
Payment Service
```

### If test succeeds:

```text
Request → Success
```

Circuit closes again.

```text
CLOSED
```

### If test fails:

```text
Request → Failure
```

Circuit opens again.

```text
OPEN
```

## State Transition Diagram

```text
           Failures Exceed Threshold
                    |
                    v
      +-------------------------+
      |         CLOSED          |
      +-------------------------+
                    |
                    v
      +-------------------------+
      |          OPEN           |
      +-------------------------+
                    |
          Wait Timeout Period
                    |
                    v
      +-------------------------+
      |       HALF-OPEN         |
      +-------------------------+
          |                |
       Success          Failure
          |                |
          v                v
      CLOSED            OPEN
```

# Example in E-Commerce

Suppose:

```text
Frontend
    ↓
Order Service
    ↓
Payment Service
```

Payment Service goes down.

### Without Circuit Breaker:

```text
1000 requests
     ↓
1000 timeouts
```

### With Circuit Breaker:

```text
First few requests fail
      ↓
Circuit opens
      ↓
Remaining requests fail immediately
```

The system remains responsive.

# Fallback Response

Circuit Breakers are often used with fallbacks.

### Example:

```text
Order Service
      ↓
Payment Service (Down)
```

Instead of showing an error:

```text
"Payment service unavailable."
```

Return:

```text
"We are processing your payment.
Please try again later."
```

or

```text
"Cached payment status."
```

This improves user experience.

# Benefits

## 1. Prevents Cascading Failures

One failed service doesn't bring down the whole system.

## 2. Faster Failure Detection

Requests fail immediately instead of waiting for long timeouts.

## 3. Improves System Stability

Healthy services continue working.

## 4. Reduces Resource Consumption

No unnecessary retries.

## 5. Better User Experience

Fallback responses can be provided.
