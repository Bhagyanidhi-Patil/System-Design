# Bulkhead Pattern

The **Bulkhead Pattern** is a design pattern used in microservices and distributed systems to **isolate failures** so that a problem in one part of the system does not affect other parts.

It is inspired by the **bulkheads in ships**.

*Isolate = Separate something so that problems in one part do not impact other parts.*

## Real-Life Analogy

A ship is divided into multiple watertight compartments called bulkheads.

```text
+---------+---------+---------+
| Room A  | Room B  | Room C  |
+---------+---------+---------+
```

If water enters one compartment:

```text
+---------+---------+---------+
| FLOODED | Room B  | Room C  |
+---------+---------+---------+
```

The water is contained within that compartment.

The entire ship does not sink.

Similarly, in software:

```text
Order Service
Payment Service
Inventory Service
Notification Service
```

If one service consumes all resources, other services should continue working.

This is the purpose of the Bulkhead Pattern.

---

# Problem Without Bulkhead

Suppose an application has:

```text
Order Service
      |
      +--> Payment Service
      |
      +--> Inventory Service
      |
      +--> Notification Service
```

All services share the same thread pool:

```text
Thread Pool = 100 Threads
```

Suddenly Payment Service becomes slow.

```text
Payment Request 1
Payment Request 2
Payment Request 3
...
Payment Request 100
```

All threads become occupied.

Result:

```text
Payment Service     → Slow
Inventory Service  → Blocked
Notification Service → Blocked
```

Even healthy services cannot respond.

This is called **resource starvation**.

---

# Solution: Bulkhead Pattern

Allocate separate resources to different services.

Instead of:

```text
Shared Thread Pool
```

Use:

```text
Payment Pool       = 40 Threads
Inventory Pool     = 30 Threads
Notification Pool  = 30 Threads
```

Now:

```text
Payment Service → Slow
```

Only Payment Pool is affected.

```text
Inventory Pool → Still Working
Notification Pool → Still Working
```

### The Failure is Isolated

#### Requires Payment

* Place Order
* Make Payment
* Refund Payment

These will fail if Payment Service is down.

#### Does NOT Require Payment

* Browse Products
* Check Inventory
* View Order History
* Track Shipment
* Receive Notifications

These can continue working.

So instead of:

```text
Entire Website ❌
```

you get:

```text
Checkout ❌
Browsing ✅
Inventory Check ✅
Order Tracking ✅
Notifications ✅
```

This is much better for users.

---

# How Bulkhead Works

Without Bulkhead:

```text
                    Shared Pool
                         |
      -----------------------------------
      |               |                |
 Payment        Inventory      Notification
```

With Bulkhead:

```text
Payment Pool
      |
Payment Service

Inventory Pool
      |
Inventory Service

Notification Pool
      |
Notification Service
```

Each service gets its own resources.

---

# Example in E-Commerce

Suppose:

```text
Frontend
    |
Order Service
    |
    +--> Payment Service
    +--> Inventory Service
    +--> Shipping Service
```

Payment Service becomes slow.

### Without Bulkhead

```text
100 Threads Total

Payment uses all 100
```

Result:

```text
Inventory Requests → Waiting
Shipping Requests → Waiting
```

Entire application slows down.

### With Bulkhead

```text
Payment Pool = 40
Inventory Pool = 30
Shipping Pool = 30
```

Payment may consume all 40 threads.

But:

```text
Inventory → Works
Shipping → Works
```

The system remains partially functional.

---

# Types of Bulkhead

## 1. Thread Pool Bulkhead

Separate thread pools for different operations.

Example:

```text
Payment Threads     = 50
Inventory Threads   = 30
Shipping Threads    = 20
```

Most common implementation.

---

## 2. Semaphore Bulkhead

Limit the number of concurrent calls.

Example:

```text
Maximum Payment Requests = 20
```

If request number 21 arrives:

```text
Rejected
```

or

```text
Wait in Queue
```

This prevents resource exhaustion.

---

# Benefits

## 1. Failure Isolation

A problem in one service does not affect others.

## 2. Better Availability

Healthy services continue functioning.

## 3. Resource Protection

Prevents one component from consuming all resources.

## 4. Improved Stability

System remains operational during partial failures.

## 5. Better Performance

Critical services receive guaranteed resources.

---

# Bulkhead vs Circuit Breaker

| Feature  | Bulkhead              | Circuit Breaker               |
| -------- | --------------------- | ----------------------------- |
| Purpose  | Isolate resources     | Stop calls to failing service |
| Prevents | Resource starvation   | Cascading failures            |
| Action   | Separate pools/limits | Open/close circuit            |
| Focus    | Resource isolation    | Failure handling              |

### Example

```text
Payment Service becomes slow
```

**Bulkhead:**

```text
Only Payment Pool affected
```

**Circuit Breaker:**

```text
Stop calling Payment Service temporarily
```

They are often used together.

---

# Bulkhead + Circuit Breaker

```text
Order Service
      |
      +--> Bulkhead
      |
      +--> Circuit Breaker
      |
      +--> Payment Service
```

Flow:

1. Bulkhead limits resource usage.
2. Circuit Breaker stops repeated failures.
3. System remains stable.

---

