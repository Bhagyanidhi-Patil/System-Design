# How would you prevent duplicate requests? 
Preventing duplicate requests is mainly about making operations idempotent so retries or repeated submissions do not create duplicate side effects.

## Why Duplicate Requests Happen

Duplicate requests can occur because of:

- User double-clicking a button
- Browser retrying requests
- Network timeout causing client retries
- Mobile apps reconnecting
- Load balancers retrying
- Message queues delivering twice
- Distributed systems race conditions

## Main Techniques to Prevent Duplicate Requests

### 1. Idempotency Keys (Best for APIs & Payments)

An idempotency key is a unique identifier sent with a request.

#### How It Works

Client generates a unique key:

```http
POST /payments
Idempotency-Key: abc123
```

Server behavior:

- First request:
  - Process normally
  - Store response with key
- Duplicate request with same key:
  - Return previous response
  - Do NOT process again

#### Flow
```
Client  
   |  
   | Request + Idempotency-Key  
   v  
Server checks storage  
   |  
   |-- Key not found --> Process request --> Save result  
   |  
   |-- Key exists --> Return saved response
```
#### Backend Pseudocode

```python
if key_exists(idempotency_key):
    return stored_response(idempotency_key)

result = process_payment()

save_key_and_response(idempotency_key, result)

return result
```

#### Best Practices

- Use UUIDs
- Expire old keys
- Store request hash also
- Tie key to user/session

#### Common Usage

- Payment gateways
- Order creation
- Financial transactions
- Booking systems
---
### 2. Database Unique Constraints

This is the simplest and strongest database-level protection against duplicates.

The database itself guarantees uniqueness.

#### What Is It?

A unique constraint means:

> “This value cannot appear twice in this column.”

#### Example 1 — User Registration

Suppose two requests try to create same email:

john@gmail.com

Database table:

```sql
CREATE TABLE users (
    id INT PRIMARY KEY,
    email VARCHAR(255) UNIQUE
);
```

#### What Happens?

- First request

  ```sql
  INSERT INTO users(email)
  VALUES('john@gmail.com');
  ```

  ✅ Success

- Second request

  ```sql
  INSERT INTO users(email)
  VALUES('john@gmail.com');
  ```

  ❌ Database rejects it.

Error:

Duplicate entry

---
### 3. Distributed Locks

Distributed locks are mainly used when multiple users or multiple servers try to access/update the same resource at the same time.

Movie ticket booking, hotel booking, flight seats, concert tickets, flash sales — these are classic distributed lock problems.

#### Problem Without Lock
```
Timeline

- User A checks seat A10 -> Available
- User B checks seat A10 -> Available

Both see seat available.

Then:

- User A books seat
- User B books seat
```
Now:

❌ Same seat booked twice

This is called: Race Condition

#### Solution Using Distributed Lock

Before booking:

```text
LOCK(seat_A10)
```

Only one request gets lock.

Flow
```
- User A
  - Acquire lock(seat_A10) ✅
  - Book seat
  - Release lock
- User B
  - Acquire lock(seat_A10) ❌
  - Wait OR fail

Then after User A completes:

Seat already booked

So duplicate booking prevented.
```
With lock:

```text
Acquire lock(order_101)
    |
Process
    |
Release lock
```

#### Why Called "Distributed" Lock?

Because:

- App may run on many servers

Example:

Server 1 -> User A request
Server 2 -> User B request

Normal in-memory lock won't work.

Need shared lock system like:

- Redis
- ZooKeeper
- etcd

accessible by all servers.

---

### 4. Debouncing (Frontend)

Prevents rapid repeated actions.

#### Example

User clicks button 10 times quickly.

Debounce ensures only one request is sent.

#### JavaScript Example

```js
button.addEventListener(
  "click",
  debounce(submitForm, 1000)
);
```

#### Best For

- Search bars
- Form submission
- UI buttons

---

### 5. Disable UI After First Click

Very common frontend protection.

Example
```
button.disabled = true;

After submission:

Disable button show loading spinner prevents double form submit duplicate checkout requests.
```
---

### 6. Request Fingerprinting

Server computes a hash from request payload.

Example
```
Hash generated from:

{
  "user": 101,
  "amount": 1000,
  "receiver": 202
}
```
Hash:
```
SHA256(payload)
```
If same hash appears within short time:

Reject duplicate.

Useful For Webhooks ,Event processing ,Background jobs.

---

### 7. Queue Deduplication

Queues sometimes deliver same message multiple times.

Example:
```
Process order #101

Message sent to queue.

But due to network retry:

Process order #101
Process order #101

Same message arrives twice.
```

Without Deduplication
```
Consumer processes:

same order twice
same email twice
same payment twice
```
### Queue Deduplication Solution

Every message gets:
Deduplication ID

Example:
```
order_101

Queue stores processed IDs temporarily.

If duplicate message arrives:

Ignored automatically
```

---

### 8. Token-Based One-Time Requests

Generate single-use token.

Example
```
csrf_token = xyz123

Once used:

Mark invalid
Reject reuse
```

---
