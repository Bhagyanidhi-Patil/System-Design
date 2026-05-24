# Idempotency

Idempotency is a property of an operation where performing it multiple times has the same effect as performing it once.

---

## Everyday examples

### Pressing an elevator button:
- Press once → elevator is called.  
- Press 10 times → still just called once.

---

## In programming and APIs

An operation is idempotent if repeated requests do not create additional side effects.

### Example

Suppose an API endpoint creates a payment:

### Non-idempotent
POST /charge


If sent twice accidentally, the customer may be charged twice.

---

### Idempotent design

The client sends an idempotency key:


Idempotency-Key: abc123


The server remembers the first request result and returns the same response for retries instead of processing again.

---

## HTTP methods and idempotency

According to REST conventions:

| Method | Idempotent? | Why |
|--------|-------------|-----|
| GET    | Yes         | Fetching data repeatedly doesn’t change state |
| PUT    | Yes         | Replacing a resource with same data keeps same result |
| DELETE | Yes         | Deleting something multiple times still leaves it deleted |
| POST   | Usually No  | Repeated requests may create multiple resources |

---

## Why idempotency matters

It helps with:

- Network retries  
- Preventing duplicate payments/orders  
- Fault tolerance in distributed systems  
- Safe re-execution of operations  

---

## Idempotency in microservices

In microservices, idempotency is a key design principle that ensures a service can safely handle the same request multiple times without causing unintended side effects (like duplicate orders, double payments, or repeated state changes).

This matters a lot because microservices run in distributed systems where failures, retries, and duplicate messages are normal, not exceptions.

---

## Why idempotency is important in microservices

In real systems, requests can be repeated due to:

- network timeouts  
- client retries  
- load balancers resending requests  
- message queue re-delivery (at-least-once delivery)  
- service crashes after partial processing  

Without idempotency, each retry could cause duplicate actions.

---

### Example problem:
```
User clicks “Pay ₹500”  
Request times out  
Client retries automatically  
Two payments get processed → user is charged twice  
```
---

## Idempotency in practice

### 1. Idempotent API design (HTTP level)

Some operations are naturally idempotent:

- GET /orders/123 → safe to repeat  
- PUT /orders/123 → replaces same resource state  
- DELETE /orders/123 → deleting multiple times still results in “deleted”  

But risky ones:

- POST /orders → usually creates a new order each time  

---

### 2. Idempotency keys (most common in microservices)

A standard solution is using an idempotency key.

How it works:

Client generates a unique key:


Idempotency-Key: 7f3a-91bc

Sends request:
POST /payments


Server logic:

If key is new → process request and store result  
If key was seen before → return the stored response (do NOT reprocess)

Result:

Even if the request is retried 5 times, only one payment happens.

---

### 3. Database-level idempotency

Microservices often enforce idempotency using the database.

#### Example: Unique constraint

```sql
CREATE UNIQUE INDEX unique_order_request
ON orders(client_request_id);

If the same request is inserted twice:

First succeeds
Second fails or is ignored
```
### 4. Message queue idempotency (very important)

In systems like Kafka or RabbitMQ, messages are often delivered at least once, meaning duplicates can happen.

So consumers must be idempotent:
```
Example:

Message: “create order #123”

Consumer checks:

Has order #123 already been processed?
Yes → skip
No → create it
```
This is called deduplication.