# Designing an Idempotent POST API 

### What is Idempotency?

An operation is **idempotent** if performing it multiple times produces the same result as performing it once.

For example:

```http
POST /payments
```

Suppose a client sends a payment request.

* Network timeout occurs.
* The client doesn't know whether the payment succeeded.
* The client retries the request.

Without idempotency:

```text
Request 1 → Payment Created
Request 2 → Another Payment Created

Result:
Customer charged twice ❌
```

With idempotency:

```text
Request 1 → Payment Created
Request 2 → Same Response Returned

Result:
Customer charged only once ✅
```

---

### Why Do We Need Idempotency?

In distributed systems, retries are common because of:

* Network failures
* Client timeouts
* Load balancer failures
* Service restarts
* Message queue redelivery

A well-designed API must safely handle duplicate requests.

Common examples include:

* Payment processing
* Order creation
* Ticket booking
* Wallet transactions
* Bank transfers

---

#### Interview Definition

An **idempotent POST API** ensures that multiple identical requests produce the same effect as a single request, preventing duplicate resource creation during retries.

---

### High-Level Design

The client generates a unique **Idempotency Key** and sends it along with the request.

```http
POST /payments

Headers:
Idempotency-Key: abc123

Body:
{
  "amount": 100,
  "userId": 123
}
```

The server stores:

* Idempotency Key
* Request Hash
* Response
* Status

---

### Flow Diagram

```text
                POST /payments
                       |
                       |
         Idempotency-Key = abc123
                       |
                       v
            Check Idempotency Store
                       |
           +-----------+-----------+
           |                       |
         Found                  Not Found
           |                       |
           v                       v
 Return Stored Response     Process Request
           |                       |
           |                 Create Payment
           |                       |
           +-----------Store Result+
                       |
                       v
                 Return Response
```

---

### Database Design

Create an **Idempotency Table**.

```sql
CREATE TABLE idempotency_keys (
    key VARCHAR(255) PRIMARY KEY,
    request_hash VARCHAR(255),
    response JSON,
    status VARCHAR(20),
    created_at TIMESTAMP
);
```

Example:

| Key    | Request Hash | Status  |
| ------ | ------------ | ------- |
| abc123 | hash123      | SUCCESS |

---

## Request Processing Logic

### What problem are we solving?

Suppose a customer clicks **"Pay ₹1000"**.

The client sends:

```http
POST /payments

Amount = ₹1000
```

While processing, the network times out.

The client doesn't know whether:

* The payment succeeded
* The payment failed
* The server never received the request

So, the client retries the request.

Without idempotency:

```text
First Request
      ↓
Payment Created

Retry Request
      ↓
Another Payment Created

Customer charged twice ❌
```

To prevent duplicate payments, every request carries a unique **Idempotency-Key**.

Example:

```http
POST /payments
Idempotency-Key: abc123
```

This key uniquely identifies the payment request.

---

### First Request Flow

Assume the client sends:

```http
POST /payments

Headers:
Idempotency-Key: abc123

Body:
{
    "amount": 1000,
    "userId": 10
}
```

The server performs the following steps:

```text
Receive Request
       │
       ▼
Check Idempotency Table
       │
       ▼
Key Exists?
```

Since this is the **first request**, the key **abc123** is not present in the database.

Therefore, the server proceeds with processing the payment.

---

### Step 1: Process Payment

The server processes the payment.

```text
Call Payment Service
        ↓
Debit Account
        ↓
Create Payment Record
        ↓
PaymentID = 5001
```

---

### Step 2: Store the Response

Instead of simply returning the response to the client, the server stores it in the database.

Example Idempotency Table:

| Idempotency Key | Payment ID | Status  | Response      |
| --------------- | ---------- | ------- | ------------- |
| abc123          | 5001       | SUCCESS | JSON Response |

Stored Response:

```json
{
    "paymentId": 5001,
    "status": "SUCCESS"
}
```

---

### Step 3: Return Response

The server returns:

```http
HTTP/1.1 201 Created

{
    "paymentId": 5001,
    "status": "SUCCESS"
}
```

The payment is successfully completed.

---

### Retry Request

Now assume the client never receives the response because of a network timeout.

The client retries using the **same idempotency key**.

```http
POST /payments
Idempotency-Key: abc123
```

The server executes the following flow:

```text
Receive Request
       │
       ▼
Lookup Idempotency Key
       │
       ▼
Key Found
```

Since the key already exists, the server **does not process the payment again**.

Instead, it immediately returns the previously stored response.

```text
Database
   │
abc123
   │
Stored Response
   │
Return Response
```

Returned Response:

```json
{
    "paymentId": 5001,
    "status": "SUCCESS"
}
```

Notice that:

* The payment logic is **not executed again**.
* The customer's account is **not debited twice**.
* No new Payment ID is generated.

The client receives exactly the same response that was generated during the first successful request.

This is the essence of **idempotency**—multiple identical requests produce the **same result**, preventing duplicate operations.

---

## Handling Concurrent Requests

This is one of the most common follow-up questions in system design interviews.

Imagine two identical requests arrive at **almost the same time**.

```text
Request A

POST /payments
Idempotency-Key: abc123


Request B

POST /payments
Idempotency-Key: abc123
```

Both requests reach the server simultaneously.

---

### What Happens Without Protection?

Suppose the server follows this logic:

```text
Check Idempotency Key
        ↓
If Key Not Found
        ↓
Create Payment
```

### Timeline

```text
Time T1

Request A
     │
Check Key
     │
Not Found
```

```text
Time T2

Request B
     │
Check Key
     │
Also Not Found
```

Since neither request has inserted the idempotency key into the database yet, **both requests believe they are the first request**.

Both continue processing.

```text
Request A
      ↓
Create Payment
PaymentID = 5001


Request B
      ↓
Create Payment
PaymentID = 5002
```

Result:

```text
Two payments are created ❌
```

This problem is known as a **Race Condition**.

A race condition occurs when multiple requests access and modify shared data simultaneously, leading to duplicate or inconsistent results.

---

### Solution 1 – Use a Unique Constraint

To prevent duplicate processing, store the idempotency key in a database table where it is defined as a **PRIMARY KEY** or has a **UNIQUE constraint**.

```sql
CREATE TABLE idempotency (
    idempotency_key VARCHAR PRIMARY KEY,
    status VARCHAR,
    response JSON
);
```

Since the `idempotency_key` is the primary key, the database guarantees that the same key can be inserted only once.

---

### Flow

Both requests arrive at the same time.

**Request A**

```text
INSERT abc123

SUCCESS
```

**Request B**

```text
INSERT abc123

Fails

Duplicate Key Error
```

Because Request B cannot insert the same key, it immediately knows another request is already processing the operation.

Only **Request A** proceeds to create the payment.

This guarantees that **only one request can process the payment**, eliminating duplicate transactions.

---

### Better Design – Store Request State

A production-grade system doesn't just store the idempotency key.

It also stores the **current processing state** of the request.

Instead of storing only:

| Key    |
| ------ |
| abc123 |

Store:

| Key    | Status      |
| ------ | ----------- |
| abc123 | IN_PROGRESS |

Possible status values are:

```text
IN_PROGRESS

SUCCESS

FAILED
```

---

### Why Do We Need `IN_PROGRESS`?

Suppose **Request A** starts processing.

```text
Insert Key
     │
abc123
     │
Status = IN_PROGRESS
```

Payment processing may take several seconds.

During this time, **Request B** arrives.

Instead of trying to create another payment, it checks the idempotency table.

```text
abc123

Status = IN_PROGRESS
```

The server now knows:

> Another request is already processing this payment.

Therefore, it should not process the payment again.

---

### Possible Responses

### Option 1 (Most Common)

Return:

```http
HTTP 409 Conflict
```

or

```http
HTTP 425 Too Early
```

This tells the client that the request is already being processed and it should retry later.

---

### Option 2

Wait until **Request A** completes.

Once the status changes to **SUCCESS**, simply return the stored response.

---

### Option 3

Retry internally after a short delay.

If the request finishes successfully, return the stored response.

---

Once Request A completes, the database is updated.

```text
Status

IN_PROGRESS
      ↓
SUCCESS
```

Now, every future retry simply returns the stored response without processing the payment again.

---

### Timeline Example

```text
Time 0

Request A
      │
Insert Key
      │
Status = IN_PROGRESS
```

```text
Time 1

Request B
      │
Reads Key
      │
Status = IN_PROGRESS
      │
Returns HTTP 409
```

```text
Time 3

Payment Completed
      │
Status = SUCCESS
      │
Response Stored
```

```text
Time 5

Request B Retries
      │
Status = SUCCESS
      │
Return Stored Response
```

No duplicate payment is created because only one request was allowed to process the payment.

---

# Request Hash Validation

Another important interview topic is **preventing misuse of the same idempotency key**.

Suppose the first request is:

```http id="5fz2r8"
POST /payments
Idempotency-Key: abc123
```

Request Body:

```json id="p3cktk"
{
    "amount": 100
}
```

The server processes the request and stores:

| Key    | Amount |
| ------ | ------ |
| abc123 | 100    |

---

Later, the client (or due to a bug) sends another request using the **same idempotency key**.

```http id="szqjng"
POST /payments
Idempotency-Key: abc123
```

But this time the request body is different.

```json id="lk62s9"
{
    "amount": 500
}
```

This is **not a retry**.

It is a **completely different request** that is incorrectly reusing the same idempotency key.

If the server simply returns the previously stored response, it ignores the new request.

If it processes the request as a new payment, it violates idempotency.

Therefore, the server needs a way to determine whether the incoming request is truly a retry of the original request.

The solution is to store a **hash of the request payload**.

---

## How Request Hash Validation Works

When the first request arrives:

```json id="a0w7ul"
{
    "amount": 100,
    "currency": "INR",
    "userId": 10
}
```

The server computes a hash of the request body.

```text id="0swyah"
SHA-256(request body)
```

For example:

```text id="qmt06i"
Hash = xyz123
```

The database stores:

| Idempotency Key | Request Hash | Status  |
| --------------- | ------------ | ------- |
| abc123          | xyz123       | SUCCESS |

Now every request with the same idempotency key can be validated against this stored hash.

---

### Validation Process

Whenever a request arrives with an existing idempotency key, the server performs the following steps:

1. Look up the idempotency key in the database.
2. Compute the SHA-256 hash of the incoming request body.
3. Compare the newly computed hash with the stored hash.

---

### Case 1: Hashes Match

Incoming Request:

```json id="okifjp"
{
    "amount": 100,
    "currency": "INR",
    "userId": 10
}
```

Computed Hash:

```text id="jpokmz"
xyz123
```

Stored Hash:

```text id="uwmjlwm"
xyz123
```

Since both hashes are identical, the server concludes that this is a genuine retry of the original request.

Therefore, it safely returns the previously stored response without executing the payment logic again.

```text id="rdh8vx"
Hash Match
      │
      ▼
Valid Retry
      │
      ▼
Return Stored Response
```

---

### Case 2: Hashes Do Not Match

Incoming Request:

```json id="n8zofm"
{
    "amount": 500,
    "currency": "INR",
    "userId": 10
}
```

Computed Hash:

```text id="8mj0zd"
abc789
```

Stored Hash:

```text id="k53xsr"
xyz123
```

Since the hashes are different, the server knows that the client is trying to reuse the same idempotency key for a different request.

This is considered an invalid request.

The server rejects it.

```http id="50iz0g"
HTTP/1.1 400 Bad Request
```

Response:

```json id="fjlngv"
{
    "error": "Idempotency key reused with different request payload"
}
```

This prevents accidental bugs, client errors, or malicious attempts to reuse an idempotency key for multiple different operations.

---

### Why Is Request Hash Validation Important?

Without request hash validation:

* The same idempotency key could be reused for different requests.
* The server might return an incorrect old response.
* Different operations could accidentally share the same idempotency key.
* This could lead to incorrect business behavior and data inconsistencies.

By storing and validating the **SHA-256 hash of the request body**, the server ensures that an idempotency key always represents **one unique logical request**.

Only requests with the **same idempotency key** and the **same request payload** are treated as valid retries.

---

## End-to-End Flow

```text
                          Client
                             │
      POST /payments (Idempotency-Key = abc123)
                             │
                             ▼
                 Check Idempotency Table
                             │
              ┌──────────────┴──────────────┐
              │                             │
       Key Not Found                 Key Exists
              │                             │
              ▼                             ▼
 Insert Record (Status = IN_PROGRESS)  Compare Request Hash
              │                             │
              ▼                  ┌──────────┴──────────┐
      Process Payment            │                     │
              │             Hash Match         Hash Mismatch
              ▼                  │                     │
      Store Response             ▼                     ▼
     Status = SUCCESS     Return Stored Response  Return 400 Bad Request
              │
              ▼
      Return HTTP 201 Created
```

### Summary

1. The client sends a payment request with a unique **Idempotency-Key**.
2. The server first checks whether the key already exists.
3. If the key does **not** exist:

   * Insert the key with **Status = IN_PROGRESS**.
   * Process the payment.
   * Store the response and update the status to **SUCCESS**.
   * Return **HTTP 201 Created**.
4. If the key **already exists**:

   * Compare the hash of the incoming request with the stored request hash.
   * If the hashes match, it is a valid retry, so return the previously stored response.
   * If the hashes do not match, reject the request with **HTTP 400 Bad Request**, since the same idempotency key is being reused with a different payload.

---

## Idempotency in Distributed Systems

### Why Is a Shared Store Needed?

Using idempotency on a **single server** is straightforward because all requests are handled by the same application instance.

However, in a production environment, applications are usually deployed across multiple servers behind a load balancer.

```text
               Load Balancer
                     │
          ┌──────────┴──────────┐
          │                     │
      Server A             Server B
```

Suppose the first request is routed to **Server A** and the retry request is routed to **Server B**.

If each server stores idempotency keys only in its local memory:

* Server A processes the first request.
* Server B has no knowledge of the previous request.
* Server B processes the retry again.

This results in **duplicate processing**, defeating the purpose of idempotency.

Therefore, all application instances must use a **shared storage** that is accessible from every server.

```text
               Load Balancer
                     │
          ┌──────────┴──────────┐
          │                     │
      Server A             Server B
          │                     │
          └──────────┬──────────┘
                     │
             Shared Storage
         (Redis / SQL Database)
```

Every server first checks the shared storage before processing the request.

---

### Storage Options

The two most common choices are **Redis** and a **SQL Database**.

### Option 1: Redis

Redis is the most commonly used storage for idempotency keys.

```text
Client
    │
Idempotency Key
    │
    ▼
 Redis
```

### Benefits

* Extremely fast in-memory lookups.
* Very low latency.
* Built-in support for TTL (Time-To-Live).
* Ideal for high-throughput applications.

Redis is commonly used for APIs where very fast request processing is required.

---

### Option 2: SQL Database

Examples:

* PostgreSQL
* MySQL

### Benefits

* Strong consistency.
* ACID transaction guarantees.
* Durable storage.
* Suitable for financial systems.

For payment systems, many companies prefer storing idempotency records in the same database transaction as the payment record to ensure maximum consistency.

---

### Using Redis to Prevent Concurrent Processing

Redis provides the **SETNX (Set if Not Exists)** command.

Example:

```text
SETNX abc123 IN_PROGRESS
```

The command means:

> Store the key only if it does not already exist.

---

### How SETNX Works

Suppose two identical requests arrive simultaneously.

#### Request A

```text
SETNX abc123 IN_PROGRESS

Result: SUCCESS
```

Since the key does not exist, Redis stores it successfully.

---

#### Request B

```text
SETNX abc123 IN_PROGRESS

Result: FAIL
```

Because the key already exists, Redis rejects the second request.

Therefore:

* Request A continues processing.
* Request B immediately knows another request is already processing the same operation.

Only **one request** is allowed to execute the business logic.

---

### After Successful Processing

Once payment processing completes, update the stored value.

```text
abc123
   │
Status = SUCCESS
Response = {...}
```

Future retry requests simply read this stored response instead of executing the payment again.

---

### Expiration Strategy (TTL)

An important interview question is:

> **Should idempotency keys be stored forever?**

The answer is **No**.

If keys are never removed, the storage keeps growing indefinitely, consuming memory or database space.

Therefore, idempotency records should have a **Time-To-Live (TTL)**.

---

### Typical TTL Values

| Operation        | Recommended TTL |
| ---------------- | --------------- |
| Payments         | 24–48 hours     |
| Orders           | 24 hours        |
| Wallet Transfers | 7 days          |

The TTL depends on the business requirements and how long clients are expected to retry requests.

---

### Redis Example

Redis provides the **EXPIRE** command.

```text
EXPIRE abc123 86400
```

Here:

* **86400 seconds = 24 hours**

After 24 hours, Redis automatically removes the idempotency key.

This keeps the storage clean and prevents unnecessary memory usage.

---

### Failure Scenario

Consider the following situation.

The server receives a payment request.

It inserts the idempotency key into the database.

```text
abc123

Status = IN_PROGRESS
```

Before payment processing completes, the server crashes.

```text
Insert Key
      │
Status = IN_PROGRESS
      │
Server Crash ❌
```

Now the client retries the request.

The retry checks the database and finds:

```text
abc123

Status = IN_PROGRESS
```

The problem is that this request may remain in the **IN_PROGRESS** state forever.

Since the original server has crashed, no one will ever update the status to **SUCCESS** or **FAILED**.

Every future retry will continue seeing **IN_PROGRESS**, causing the request to become permanently blocked.

---

### Solution: Detect Stale Requests

To solve this problem, store the timestamp when the request enters the **IN_PROGRESS** state.

Example:

```text
Status = IN_PROGRESS

created_at = 10:00 AM
```

Whenever another request arrives, calculate:

```text
Current Time - created_at
```

If the elapsed time exceeds a predefined timeout, assume that the original request has failed.

Example:

```text
Current Time = 10:06 AM

created_at = 10:00 AM

Elapsed Time = 6 minutes
```

If the timeout is configured as **5 minutes**, the request is considered **stale**.

The server can then:

* Mark the previous request as expired or failed.
* Retry processing safely.
* Update the idempotency record with the new result.

This prevents requests from remaining stuck in the **IN_PROGRESS** state forever.

---

