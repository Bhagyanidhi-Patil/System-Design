# How do you ensure consistency across services?

## What is consistency?

Consistency means that all services in a distributed system see the same data state (immediately or eventually).

`Example:`

Consider an e-commerce application:

1. `User places an order.`
2. `Order Service creates the order.`
3. `Inventory Service reduces stock.`
4. `Payment Service processes payment.`

Suppose Order Service succeeds but Inventory Service fails.

Now:
```
* Order Service says the order exists.
* Inventory Service says the stock is unchanged.
```
The system is now in an inconsistent state.

The interviewer's question is:

`"How would you design the system so that all services remain consistent even when failures occur?"`

---

### First explain the challenge

In a monolithic application:

```
BEGIN TRANSACTION

Insert Order
Update Inventory
Process Payment

COMMIT
```

- Everything succeeds or rolls back.
- Easy.



In microservices:
```
Order Service
Inventory Service
Payment Service
```
`Each service has its own database.`

- A single database transaction cannot easily span across multiple services and databases.

- This creates consistency challenges because one service may successfully complete its operation while another service fails, leaving the overall system in an inconsistent state.

---

##  Approaches to ensure consistency
### 1. Distributed Transactions (2PC)

When multiple services need to update their data as part of a single business operation, we need a way to keep all services consistent.

For example, in an e-commerce application:

* Order Service creates an order.
* Inventory Service reserves stock.
* Payment Service charges the customer.

If one service succeeds and another fails, the system can become inconsistent. To avoid this, we use **Distributed Transactions**.

### What is Two-Phase Commit (2PC)?

**Two-Phase Commit (2PC)** is a protocol that ensures either:

* **All services commit the transaction**, or
* **All services roll back the transaction**

No service is allowed to commit independently.

### Components

### Coordinator

A central component that manages the transaction and communicates with all participating services.

### Participants

The services involved in the transaction.

Example:

```
Coordinator
    |
    +--> Order Service
    +--> Inventory Service
    +--> Payment Service
```

### Phase 1: Prepare Phase (Voting Phase)

The coordinator asks every service:

` "Can you commit this transaction?"`

Each service:
```
1. Performs all validations.
2. Locks required resources.
3. Writes temporary transaction data.
4. Does NOT permanently save changes yet.
5. Replies:

   * YES (Ready to commit)
   * NO (Cannot commit)
```

### Example

Customer places an order.

Coordinator sends:

```
Prepare Transaction
```

#### Order Service

* Creates order in pending state.
* Locks order record.
* Replies YES.

#### Inventory Service

* Checks stock availability.
* Reserves stock.
* Replies YES.

#### Payment Service

* Verifies payment can be processed.
* Replies YES.

Result:

```
Order Service      -> YES
Inventory Service  -> YES
Payment Service    -> YES
```

### Phase 2: Commit Phase

If **all services return YES**, coordinator sends:

```
COMMIT
```

Each service permanently saves changes.

### Example

Order Service:

```
Order = CONFIRMED
```

Inventory Service:

```
Stock reduced
```

Payment Service:

```
Payment charged
```

Transaction completes successfully.

## Flow Diagram

### Successful Transaction

```text
Coordinator
    |
    |---- Prepare ----> Order Service
    |---- Prepare ----> Inventory Service
    |---- Prepare ----> Payment Service

Responses:
YES, YES, YES

Coordinator
    |
    |---- Commit ----> Order Service
    |---- Commit ----> Inventory Service
    |---- Commit ----> Payment Service
```

---

### Failed Transaction

```text
Coordinator
    |
    |---- Prepare ----> Order Service
    |---- Prepare ----> Inventory Service
    |---- Prepare ----> Payment Service

Responses:
YES, YES, NO

Coordinator
    |
    |---- Rollback ---> Order Service
    |---- Rollback ---> Inventory Service
    |---- Rollback ---> Payment Service
```
---

### 2. Saga Pattern (Most Important)

The **Saga Pattern** is one of the most commonly used approaches for maintaining consistency in microservices. 

### Idea

Instead of having one large distributed transaction, the Saga Pattern breaks the business process into multiple **local transactions**.

* Each service performs its own transaction independently.
* After completing its work, a service triggers the next service.
* If any step fails, previously completed steps are undone using **compensating actions**.

This helps maintain consistency without requiring a global transaction.

### Order Processing Example

Consider an online shopping application.

### Step 1: Create Order

* Order Service creates an order.
* Transaction succeeds.

### Step 2: Reserve Inventory

* Inventory Service reserves the required products.
* Transaction succeeds.

### Step 3: Process Payment

* Payment Service attempts to charge the customer.
* Transaction fails.

Since the payment failed, compensating actions are executed:

1. Release Inventory
2. Cancel Order

The system returns to a consistent state.

### Saga Workflow

```text
Create Order
      ↓
Reserve Inventory
      ↓
Process Payment
      ↓
Ship Product
```

### Failure Scenario

If payment processing fails:

```text
Process Payment Failed
      ↓
Release Inventory
      ↓
Cancel Order
```

If shipping fails after payment succeeds:

```text
Ship Product Failed
      ↓
Refund Payment
      ↓
Release Inventory
      ↓
Cancel Order
```

### Advantages

### 1. Highly Scalable

* Services operate independently.
* No central transaction coordinator is required.
* Suitable for large-scale distributed systems.

### 2. Works Well with Microservices

* Each service owns its own database.
* Supports independent deployment and scaling.

### 3. No Distributed Locks

* Services do not need to lock resources across the entire system.
* Better performance and availability compared to 2PC.

### Disadvantages

### 1. Complex Implementation

* Compensating actions must be carefully designed.
* Failure handling can become complicated as the workflow grows.

### 2. Temporary Inconsistency

* During execution, some services may have completed their transactions while others have not.
* The system may remain temporarily inconsistent until compensation completes.

---

## 3. Event-Driven Architecture

Event-Driven Architecture (EDA) is a design pattern where services communicate by publishing and consuming events instead of making direct synchronous calls to each other.

This approach helps build scalable, loosely coupled, and resilient distributed systems.

### Common Event Brokers

Events are typically exchanged through a message broker such as:

* Kafka
* RabbitMQ
* Pulsar

These systems act as intermediaries between producers and consumers of events.

### How It Works

A service publishes an event whenever something important happens.

Other services that are interested in that event subscribe to it and perform their own actions independently.

### Example: Order Processing System

#### Step 1: Order Creation

Order Service creates an order and publishes an event:

```text
Order Created
```

#### Step 2: Inventory Processing

Inventory Service listens for the "Order Created" event and reserves stock.

#### Step 3: Payment Processing

Payment Service also listens for the same event and processes payment.

## Workflow Diagram

```text
Order Service
      |
      v
    Kafka
   /     \
Inventory  Payment
```

### Detailed Flow

```text
Customer Places Order
          |
          v
    Order Service
          |
          v
  Publish Event:
   "Order Created"
          |
          v
        Kafka
       /     \
      /       \
     v         v
Inventory   Payment
 Service    Service
```

### Benefits

### 1. Loose Coupling

Services do not directly call each other.

Instead of:

```text
Order Service
      |
      +--> Inventory Service
      |
      +--> Payment Service
```

We have:

```text
Order Service
      |
      v
    Kafka
   /     \
Inventory  Payment
```

This means:

* Services can evolve independently.
* New consumers can be added without modifying existing services.
* Failures in one service do not directly impact others.

### 2. Better Scalability

* Consumers can be scaled independently.
* Multiple instances can process events in parallel.
* Suitable for high-throughput systems.

### 3. Improved Fault Tolerance

* Events can be persisted in the broker.
* If a service is temporarily unavailable, it can process events later when it recovers.

### Challenges

### 1. Eventual Consistency

Data across services may not become consistent immediately.

### 2. Debugging Complexity

Tracing a request across multiple events and services can be difficult.

### 3. Duplicate Event Handling

Consumers must be designed to handle duplicate events safely.

---

## 4. Eventual Consistency

Eventual Consistency is a consistency model commonly used in large-scale distributed systems. Instead of ensuring that all services have the latest data immediately, the system guarantees that all replicas will become consistent after some time.

In other words:

> If no new updates occur, all services will eventually have the same data.

This approach improves scalability, availability, and performance in distributed systems.

### Example: Social Media Post

Suppose you create a new post on a social media platform.

### Immediately After Posting

* Your profile page shows the new post.
* The post is successfully stored.

However, followers may not see the post instantly because the update needs to propagate to multiple servers.

### A Few Seconds Later

* Followers begin seeing the post.
* News feeds are updated.
* Search indexes are refreshed.

### Final State

After a short period, every user sees the same data.

```text id="v6h29n"
User Creates Post
        |
        v
   Profile Updated
        |
        v
Data Replication
        |
        v
Followers See Post
        |
        v
System Becomes Consistent
```

### Why Use Eventual Consistency?

Strong consistency often requires:

* Distributed locks
* Synchronous communication
* Waiting for all replicas to update

These operations can slow down the system.

Eventual consistency avoids these bottlenecks by allowing updates to propagate asynchronously.

### Advantages

### 1. High Availability

Services can continue operating even if some nodes are temporarily unavailable.

### 2. Better Scalability

Systems can scale to millions of users without waiting for all databases to synchronize immediately.

### 3. Faster Response Times

Users receive responses quickly because updates do not need to be confirmed by every replica before completing.

### Disadvantages

### 1. Temporary Inconsistency

Different users may see different versions of data for a short period.

### 2. Complex Conflict Resolution

Concurrent updates may require conflict-handling mechanisms.

### 3. Difficult to Reason About

Developers must account for situations where data has not yet propagated across the system.

### Example Systems Using Eventual Consistency

Many large-scale distributed systems use eventual consistency, including:

* Amazon
* Netflix
* Uber

These platforms prioritize scalability and availability while accepting short periods of temporary inconsistency.

---

## 5. Idempotency

Idempotency is a critical concept in distributed systems and system design interviews.

An operation is said to be **idempotent** if performing it multiple times produces the same result as performing it once.

In distributed systems, network failures, timeouts, and retries are common. Without idempotency, duplicate requests can lead to incorrect results such as double payments, duplicate orders, or multiple inventory reservations.

### Why Idempotency is Important

Consider a payment service.

### Scenario

1. Customer initiates payment.
2. Payment Service successfully charges the customer.
3. Response is lost due to a network timeout.
4. Client assumes the request failed.
5. Client retries the request.

Without idempotency:

```text id="h7r8k2"
Charge Payment
Charge Payment Again
```

Result:

```text id="n4w2zp"
Customer Charged Twice
```

This creates data inconsistency and a poor user experience.

### Solution: Idempotency Key

To prevent duplicate processing, each request is assigned a unique identifier.

Common choices include:

* Transaction ID
* Request ID
* Payment ID
* Order ID

Example:

```text id="p8s6jm"
payment_id = 12345
```

When a request arrives:

1. Check whether the ID has already been processed.
2. If not processed, execute the operation and store the ID.
3. If already processed, return the previous result without executing again.

### Workflow

```text id="x5t9rv"
Client Request
      |
      v
payment_id = 12345
      |
      v
Payment Service
      |
      +--> Not Seen Before
      |        |
      |        v
      |   Process Payment
      |
      +--> Already Processed
               |
               v
       Return Previous Result
```

### Example

### First Request

```text id="e2c4qn"
POST /payment
payment_id = 12345
```

Payment is processed successfully.

### Retry Request

```text id="m7y8kl"
POST /payment
payment_id = 12345
```

Payment Service detects that the request has already been processed and ignores the duplicate operation.

Result:

```text id="u3p5cd"
Only One Payment Charged
```

### Real-World Uses

### Payments

* Prevent duplicate charges.

### Order Processing

* Prevent duplicate orders.

### Inventory Management

* Prevent reserving stock multiple times.

### Message Processing

* Prevent handling the same event repeatedly.

### Advantages

### 1. Prevents Duplicate Operations

Ensures retries do not cause incorrect updates.

### 2. Improves Reliability

Clients can safely retry failed requests.

### 3. Supports Distributed Systems

Helps maintain consistency despite network failures and message duplication.

### Challenges

### 1. Storage Overhead

Processed request IDs must be stored and tracked.

### 2. Expiration Management

Old idempotency keys need cleanup after a certain period.

### 3. Additional Logic

Services must implement duplicate detection mechanisms.

---

## 6. Outbox Pattern

The **Outbox Pattern** is a common solution used in microservices to ensure reliable event delivery between a database and a message broker such as Kafka.

It is a very common follow-up question in system design interviews, especially when discussing Event-Driven Architecture and Saga Pattern.

### The Problem

Consider the following workflow:

1. Insert Order into the database.
2. Publish an "Order Created" event to Kafka.

```text id="d8x2pq"
Update Database
      ↓
Publish Kafka Event
```

What happens if:

* Database update succeeds
* Kafka publish fails

Example:

```text id="a4k7mn"
Order Inserted Successfully
Kafka Publish Failed
```

Now the system becomes inconsistent:

* Order exists in the database.
* Other services never receive the event.
* Inventory, Payment, and Shipping services may never start processing.

This creates a reliability problem.

### Solution: Outbox Pattern

Instead of directly publishing to Kafka, the service stores the event inside an **Outbox Table** as part of the same database transaction.

### Database Transaction

```text id="p3v9rt"
Database Transaction
   ├── Order Inserted
   └── Event Saved
```

Since both operations occur within a single database transaction:

* Either both succeed.
* Or both fail.

This guarantees consistency.

### Wrong Understanding

Many people assume that the Outbox Pattern makes the following happen as a single transaction:

```text
Database Update
      +
Kafka Publish
```

This is **NOT** what the Outbox Pattern does.

**If database update and Kafka publishing were part of the same transaction**, a distributed transaction would be required between:

* Database
* Kafka

Distributed transactions are complex, slow, and generally avoided in microservice architectures.

---

### What Actually Happens

The real transaction is:

```text
Database Transaction
    ├── Insert Order to DB
    └── Insert Outbox Event
```

Both operations are written to the **same database** within a **single ACID transaction**.

**Example:**

```sql
BEGIN;

INSERT INTO orders (...);

INSERT INTO outbox
(event_type, payload, status)
VALUES
('OrderCreated', '{...}', 'PENDING');

COMMIT;
```

`If the transaction commits successfully:`

```text
Orders Table  ✓
Outbox Table  ✓
```

`If the transaction fails:`

```text
Orders Table  ✗
Outbox Table  ✗
```

Both operations succeed or fail together.

---

### Then What About Kafka?

After the database transaction commits:

```text
Orders Table
Outbox Table
      |
      v
Background Worker
      |
      v
Publish to Kafka
```

`A separate background worker continuously reads pending records from the Outbox Table and publishes them to Kafka.`

Workflow:

```text
Order Created
      ↓
Outbox Entry Created
      ↓
DB Commit
      ↓
Worker Reads Outbox
      ↓
Publish to Kafka
      ↓
Mark Event as SENT
```

This approach guarantees reliable event delivery without requiring a distributed transaction between the database and Kafka.

## Architecture

```text id="k8m5zd"
Order Service
     |
     +--> Orders Table
     |
     +--> Outbox Table
              |
              v
            Kafka
```

### Workflow

### Step 1: Create Order

Order Service receives a request.

### Step 2: Single Database Transaction

Within one transaction:

```text id="y2w7hs"
Insert Order
Insert Outbox Event
Commit Transaction
```

Example:

**Orders Table**

| Order ID | Status  |
| -------- | ------- |
| 101      | Created |

**Outbox Table**

| Event ID | Event Type   | Status  |
| -------- | ------------ | ------- |
| 1        | OrderCreated | Pending |

### Step 3: Background Worker

A background process continuously scans the Outbox Table.

```text id="z6r3pc"
Read Pending Events
       ↓
Publish to Kafka
       ↓
Mark Event as Sent
```

### Step 4: Event Consumption

Other services receive the event and continue processing.

```text id="v4s8lt"
Order Service
      ↓
Outbox Table
      ↓
Kafka
     / | \
    /  |  \
Inventory Payment Shipping
```

### Failure Handling

### Scenario 1: Service Crash After Database Commit

```text id="f1n6qb"
Order Inserted
Event Stored
Service Crashes
```

No problem.

The event remains in the Outbox Table.

When the service recovers, the background worker publishes the event.

### Scenario 2: Kafka Temporarily Down

```text id="g5m9wc"
Order Inserted
Event Stored
Kafka Unavailable
```

Background worker keeps retrying until Kafka becomes available.

No event is lost.



---