## Additional Disadvantages of Microservices

- Complexity: More moving parts mean more complexity in development, testing, and deployment.
- DevOps overhead: Requires strong DevOps practices for service orchestration, monitoring, and logging.
- Data consistency: Managing data consistency across services is harder.
- Tight coupling in a microservices system can create significant problems, even though microservices are ideally designed to be loosely coupled and independently deployable.

---

## Too Many Dependencies (Tight Coupling)

- Microservices are ideally loosely coupled and meant to work independently.
- But if they depend too much on each other, they become tightly connected.
- Then, one small request may trigger many other services (can have lots of dependency to each other).
- If one service is slow or breaks, the whole process may fail.
- Latency can increase if we don't divide the component properly.

### Example

```text
You place an order
    ↓
Order Service asks User Service
    ↓
User Service asks Auth Service
    ↓
Auth Service is down
    ↓
Your order fails
```

---

## Breaking Changes Between Services

### Imagine This

- You have Service A and Service B.
- Service A asks Service B for some data.
- Earlier, Service B used to return:

```json
{
  "status": "a"
}
```

Now, you update Service B, and it starts returning:

```json
{
  "status": "a1"
}
```

But Service A still expects `"a"`, not `"a1"`.

So when Service A sees `"a1"`, it doesn’t understand it — this can break things.

---

## Why This Is a Problem

- Microservices depend on APIs to talk to each other.
- If one service changes its output without telling others, it can cause failures, bugs, or wrong behavior.

This is called a **breaking change**.

---

# ***IMP*** Transaction Management is Difficult

## What is Transaction Management?

It means:

> "Do all steps together, or none at all."

Like:

- Take money from wallet 💰
- Reduce item stock 📦
- Save order 🧾

If one fails, cancel everything. ✅❌

---

## In Monolithic Architecture

- Easy to manage because everything is in one database.
- You can use a single transaction using built-in support (like from Spring, Hibernate, etc.).
- If anything fails, the database rolls back.

---

## In Microservices

This is much harder because:

- Each service has its own database.
- There's no shared transaction across services.