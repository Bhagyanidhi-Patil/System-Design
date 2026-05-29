# Event-Driven System

An event-driven system is an architecture where services communicate by producing and reacting to events.

An event is simply:

- “Something important happened.”

> “In an event-driven system, services communicate asynchronously using events through a message broker like Kafka or RabbitMQ. Producers emit events, and multiple consumers independently process them. This architecture improves scalability, loose coupling, reliability, and fault tolerance. Key concepts include topics, partitions, consumer groups, idempotency, ordering, retries, and eventual consistency. Event-driven systems are commonly used in microservices, real-time analytics, notifications, and large-scale distributed applications.”

## Examples

- User placed an order
- Payment completed
- File uploaded
- Ride booked
- Inventory updated

Instead of one service directly calling another synchronously, services publish events to a broker/message system, and other services consume them asynchronously.

## Core Components

### A. Event Producer

The service that generates events.

Example:

- Order Service emits: `OrderPlaced`

### B. Event Broker / Message Queue

Acts as the middleman.

Popular technologies:

- Apache Kafka
- RabbitMQ
- Amazon SQS
- Apache Pulsar

Responsibilities:

- Store events
- Deliver events
- Retry failed deliveries
- Scale consumers

### C. Event Consumer

Services listening to events.

Example:

- Notification Service consumes: `OrderPlaced`
- Analytics Service consumes: `OrderPlaced`
- Inventory Service consumes: `OrderPlaced`

## Basic Flow

```text
User Places Order
       |
       v
Order Service
       |
 emits OrderPlaced event
       |
       v
Message Broker
   /      |      \
  v       v       v
Inventory Notification Analytics
 Service     Service    Service
```

## Why Companies Use Event-Driven Architecture

### A. Loose Coupling

Services don’t directly depend on each other.

Without EDS:
```
Order Service -> Notification Service
              -> Inventory Service
              -> Analytics Service
```
With EDS:
```
Order Service -> Event Broker
```
Consumers independently subscribe.

### B. Scalability

Consumers scale independently.

If notifications spike:

- scale Notification consumers only

### C. Reliability

Broker can:

- persist messages
- retry delivery
- replay events

### D. Asynchronous Processing

User gets faster response.

Instead of waiting:

- Order -> Payment -> Inventory -> Email

You do:

- Order accepted quickly
- Background services process later

## Event Types

### A. Notification Events

Only indicate something happened.

Example:

```json
{
  "event": "OrderPlaced",
  "orderId": 123
}
```

Consumer fetches extra data if needed.

### Event-Carried State Transfer

Event includes all required data.

```json
{
  "event": "OrderPlaced",
  "orderId": 123,
  "userId": 77,
  "items": [...]
}
```

Reduces extra DB calls.

## Common Messaging Models

### A. Queue (Point-to-Point)

One consumer processes message.

Example:

```text
Producer -> Queue -> One Worker
```

Used for:

- background jobs
- image processing
- email sending

Example systems:

- Amazon SQS
- RabbitMQ

### B. Publish-Subscribe (Pub/Sub)

Multiple consumers receive same event.

```text
            -> Notification
Producer    -> Topic -> Analytics
            -> Inventory
```

Used for:

- microservices communication
- analytics
- real-time systems

Example systems:

- Apache Kafka
- Google Cloud Pub/Sub

## Delivery Guarantees

Interviewers LOVE this section.

### A. At Most Once

- Message may be lost.
- No retries.

Used when occasional loss acceptable.

Example:

- analytics logs

### B. At Least Once

- Message retried until success.

Risk:

- duplicates possible

Most common in industry.

### C. Exactly Once

- Message processed exactly once.

Hard and expensive.

Usually achieved using:

- idempotency
- transactions
- deduplication

Common with:

- payments
- banking



