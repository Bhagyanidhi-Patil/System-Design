# How Do You Make a System Idempotent?

## What is Idempotency?

Idempotency means that performing the same operation multiple times has the same effect as performing it once.

In distributed systems, failures such as network timeouts, service crashes, or temporary unavailability are common. To handle these failures, clients often retry requests. Without idempotency, these retries can cause duplicate operations.

For example, if a payment request is retried due to a timeout, the customer may be charged multiple times. Idempotency ensures that repeated requests do not create duplicate side effects.

---

## Why is Idempotency Important?

Idempotency helps:

* Prevent duplicate payments
* Prevent duplicate orders
* Prevent duplicate notifications
* Handle network failures safely
* Support reliable retry mechanisms

Since retries are unavoidable in distributed systems, idempotency is a critical design principle.

---

## Example: Payment Service

Consider a payment system.

A customer initiates a payment of ₹1000.

The request reaches the Payment Service and the payment is successfully processed. However, before the response reaches the client, a network timeout occurs.

The client assumes the request failed and retries it.

Without idempotency:

* First request charges ₹1000
* Retry charges another ₹1000

The customer is charged twice.

This is a serious consistency issue.

---

## Solution: Idempotency Key

The most common solution is to use an Idempotency Key.

The client generates a unique identifier for each operation:

Idempotency-Key: PAY-12345

### First Request

1. Payment Service receives the request.
2. Checks whether the key already exists.
3. Key is not found.
4. Processes the payment.
5. Stores:

   * Idempotency Key
   * Response
   * Transaction Details
6. Returns success response.

### Retry Request

1. Same request arrives with the same Idempotency Key.
2. Service finds the key in storage.
3. Instead of processing the payment again, it returns the previously stored response.

Result:

The customer is charged only once.

---

## How Is It Implemented?

### Step 1: Client Sends Request

POST /payments

Headers:

Idempotency-Key: PAY-12345

Body:

{
"amount": 1000,
"userId": 101
}

---

### Step 2: Service Checks Storage

The service maintains a table:

| Idempotency Key | Status  | Response          |
| --------------- | ------- | ----------------- |
| PAY-12345       | SUCCESS | Payment Completed |

---

### Step 3: Process or Return Existing Result

If key does not exist:

* Execute business logic
* Save result
* Return response

If key already exists:

* Skip execution
* Return stored response

---

## Other Techniques for Achieving Idempotency

### 1. Database Unique Constraints

Assign unique identifiers to records.

Example:

Order ID
Transaction ID
Booking ID

Database prevents duplicate insertion.

Example:

UNIQUE(transaction_id)

Even if the request is executed twice, only one record is created.

---

### 2. UPSERT Operations

UPSERT means:

* Insert if record does not exist
* Update if record already exists

This prevents duplicate data creation.

Example:

INSERT ... ON CONFLICT UPDATE

---

### 3. Event Deduplication

In event-driven systems using Kafka or message queues, duplicate events may be delivered.

Consumers maintain a list of processed event IDs.

Before processing an event:

* Check whether event ID already exists.
* If yes, ignore it.
* If no, process it and store the ID.

This guarantees that each event is handled only once logically.

---

### 4. Deduplication Table

Maintain a table containing processed request IDs.

Example:

| Request ID |
| ---------- |
| REQ-1001   |
| REQ-1002   |

Before processing:

* Check the table.
* If ID exists, ignore the request.
* Otherwise process and store the ID.

---

## Real-World Examples

### Payment Systems

Payment gateways use idempotency keys to prevent duplicate charges.

### Order Processing Systems

Order services ensure that repeated checkout requests do not create multiple orders.

### Notification Systems

Email and SMS services track message IDs to avoid sending duplicate notifications.

### Event-Driven Architectures

Kafka consumers use event IDs to avoid processing duplicate messages.

---

## Interview Answer

In distributed systems, retries are common because of network failures and timeouts. To ensure that retries do not create duplicate side effects, I make operations idempotent. The most common approach is to use an Idempotency Key, where each request carries a unique identifier. The service stores the result associated with that key. If the same request is received again, the service returns the previously stored response instead of executing the operation again. Additionally, I use database unique constraints, UPSERT operations, and event deduplication techniques to ensure that duplicate requests or messages are processed safely and only once logically.
