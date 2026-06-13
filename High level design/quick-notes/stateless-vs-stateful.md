# Stateless vs Stateful Services

## What is a Stateless Service?

A **Stateless Service** does not store client-specific data (state) between requests.

Each request is treated as a completely new request.

The service does not remember what happened in previous requests.

### Example

Suppose a user sends:

```text
Request 1:
Get Product Details
```

The service responds and forgets everything.

Later:

```text
Request 2:
Get Product Details
```

The service processes it independently.

It does not remember Request 1.

### Characteristics of Stateless Services

* No client session data stored in memory.
* Each request contains all required information.
* Easy to scale horizontally.
* Easier load balancing.
* More fault tolerant.

### Example: Product Service

```text
Client
   |
   v
Product Service
```

Request:

```http
GET /products/101
```

Response:

```json
{
  "id": 101,
  "name": "Laptop"
}
```

The service does not store anything about the client after responding.

### Real-Life Analogy

Imagine ordering coffee at a counter.

```text
Customer:
One coffee please.
```

The cashier processes the order.

After the transaction:

```text
Cashier forgets the customer.
```

The next customer is treated independently.

This is stateless behavior.

---

# What is a Stateful Service?

A **Stateful Service** stores information about previous interactions.

The service remembers data across requests.

### Example

User logs in:

```text
Request 1:
Login
```

Server creates a session:

```text
Session ID = ABC123
```

Later:

```text
Request 2:
View Profile
```

The service remembers:

```text
User = John
Session = ABC123
```

This is stateful behavior.

### Characteristics of Stateful Services

* Stores session or client data.
* Remembers previous interactions.
* More complex scaling.
* Requires session management.
* Can be harder to recover from failures.

### Example: Shopping Cart

```text
Add Item A
Add Item B
Add Item C
```

The service remembers:

```text
Cart:
- Item A
- Item B
- Item C
```

This stored information is the state.

### Real-Life Analogy

Imagine a restaurant waiter.

```text
Customer:
I ordered soup earlier.
```

Waiter remembers:

```text
Table 5
Ordered Soup
```

The waiter maintains state.

---

# Stateless Example in Microservices

```text
Frontend
   |
   v
Product Service
```

Request 1:

```http
GET /products/101
```

Request 2:

```http
GET /products/102
```

The service does not remember previous requests.

Every request is independent.

---

# Stateful Example in Microservices

```text
Frontend
   |
   v
Shopping Cart Service
```

User actions:

```text
Add Laptop
Add Mouse
Add Keyboard
```

The service stores:

```text
User Cart
```

Future requests depend on previous requests.

---

# Scaling Comparison

## Stateless

```text
        Load Balancer
              |
    ---------------------
    |         |         |
 Server1   Server2   Server3
```

Any request can go to any server.

Because:

```text
No session data stored locally.
```

### Benefits

* Easy scaling
* Easy failover
* Better load balancing

---

## Stateful

```text
User A
   |
Server 1
```

Server 1 stores session data.

If the next request goes to Server 2:

```text
Server 2
```

It may not know the user's session.

Solutions:

* Sticky Sessions
* Shared Session Storage
* Distributed Cache

### Challenges

* More complex scaling
* Session synchronization required

---

# Failure Scenario

## Stateless Service

```text
Server 1 ❌
```

Request can move to:

```text
Server 2 ✅
```

No data is lost.

---

## Stateful Service

```text
Server 1 ❌
```

If session data was stored only on Server 1:

```text
User Session Lost ❌
```

User may need to log in again.

---

# Common Examples

## Stateless Services

* Product Service
* Search Service
* Catalog Service
* Weather API
* Currency Conversion API

### Example

```http
GET /weather?city=Bangalore
```

Every request is independent.

---

## Stateful Services

* Shopping Cart Service
* Online Gaming Session
* Chat Session
* Banking Transaction Session
* User Session Management

### Example

```text
Cart:
Laptop
Mouse
Keyboard
```

The service remembers previous actions.

---

# Modern Microservices Approach

Modern systems prefer:

```text
Stateless Services
```

because they are easier to:

* Scale
* Deploy
* Load Balance
* Recover

When state is needed:

```text
Service
   |
   v
Database / Redis
```

Store state externally instead of inside service memory.

Example:

```text
Order Service (Stateless)
          |
          v
Redis / Database (Stores State)
```

This combines scalability with persistence.

---

# Quick Comparison

| Feature                     | Stateless  | Stateful |
| --------------------------- | ---------- | -------- |
| Stores Client Data          | No         | Yes      |
| Remembers Previous Requests | No         | Yes      |
| Easy to Scale               | Yes        | No       |
| Load Balancing              | Easy       | Complex  |
| Failure Recovery            | Easy       | Harder   |
| Session Management          | Not Needed | Required |

---

# Why Stateless Services Are Easy to Scale

Because any server can handle any request.

## Stateless Service

Suppose you have 3 instances of Product Service:

```text
          Load Balancer
                |
      ---------------------
      |         |         |
   Server1   Server2   Server3
```

User sends requests:

```text
Request 1 → Server1
Request 2 → Server3
Request 3 → Server2
Request 4 → Server1
```

This works because each request contains all required information.

### Example:

```http
GET /products/101
```

The server simply fetches the product and responds.

It doesn't need to remember anything from previous requests.

---

## Scaling Is Easy

Traffic increases.

You add more servers:

```text
          Load Balancer
                |
   --------------------------------
   |      |      |      |        |
 S1     S2     S3     S4      S5
```

Now requests are distributed across 5 servers.

No extra work is needed.

---

## What Happens If One Server Fails?

```text
Server2 ❌
```

The load balancer simply sends requests to:

```text
Server1
Server3
Server4
Server5
```

Users are usually unaffected because no session data was stored on Server2.

---

# Why Stateful Services Are Harder

Suppose a user logs in.

```text
User Login
     ↓
Server1
```

Server1 stores:

```text
Session = ABC123
User = John
```

Now the next request arrives.

If it goes to:

```text
Server2
```

Server2 doesn't know:

```text
Session = ABC123
```

because the session is stored on Server1.

The user may appear logged out.

---

## Shared Session Store

Store sessions in:

```text
Redis
Database
```

```text
Server1
Server2
Server3
     |
     v
   Redis
```

Now every server can read the session.

But this adds complexity and infrastructure.

