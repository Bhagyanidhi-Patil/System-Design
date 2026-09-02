A load balancer and consistent hashing both distribute requests across multiple servers, but they solve different problems and work in different ways.

`Load balancer is a component and consistent hashing is a hashing technique.`

| Aspect          | Load Balancer                                               | Consistent Hashing                                  |
| --------------- | ----------------------------------------------------------- | --------------------------------------------------- |
| Purpose         | Spread traffic evenly across servers                        | Route the same key/user/data to the same server     |
| Decision Basis  | Current server state (round-robin, least connections, etc.) | Hash of a key (user ID, cache key, object ID, etc.) |
| Request Routing | Any request can go to any healthy server                    | A given key consistently maps to a specific server  |
| State Awareness | Often stateless regarding application data                  | Preserves data locality                             |
| Scaling Impact  | Adding/removing servers may affect all traffic              | Only a small fraction of keys move                  |
| Common Uses     | Web traffic distribution, API gateways                      | Distributed caches, databases, sharding             |

****The main difference is:****

Load balancer: "Which server should handle this request right now?"  
Consistent hashing: "Which server owns this key/data?"

---

## Load Balancer and Consistent Hashing

A **load balancer can use consistent hashing**, but it is important to understand that **consistent hashing is one possible load-balancing strategy**, not the default one.

## Normal Load Balancing

Suppose you have:

```text
             Load Balancer
             /     |     \
            ↓      ↓      ↓
          S1      S2      S3
```

A common strategy is **Round Robin**:

```text
Request 1 → S1
Request 2 → S2
Request 3 → S3
Request 4 → S1
Request 5 → S2
...
```

The goal is mainly to distribute requests evenly.

---

## What Does Consistent Hashing Do?

Consistent hashing is useful when you want **the same client/key to consistently go to the same server**.

For example:

```text
User A → Load Balancer → S1
User B → Load Balancer → S2
User C → Load Balancer → S3
```

The load balancer can hash something such as:

```text
hash(user_id)
```

and use that hash to select a server.

So:

```text
User A
  ↓
hash("A")
  ↓
Consistent Hash Ring
  ↓
S1
```

The next request from User A will generally go to **S1 again**.

---

## Why Is This Useful?

Imagine your application keeps some data **in memory** on each server:

```text
S1 → User A's session
S2 → User B's session
S3 → User C's session
```

If User A's next request goes to S2:

```text
User A → S2
           ↓
     "I don't have A's session"
```

That creates a problem.

With consistent hashing:

```text
User A → S1
User A → S1
User A → S1
```

The requests tend to stay on the same server.

This is called **session affinity / sticky sessions**.

---

## Why Specifically "Consistent" Hashing?

Suppose you have:

```text
S1
S2
S3
```

and User A maps to S1.

Now you add S4.

With ordinary hashing:

```text
hash(user) % 3
```

changing to:

```text
hash(user) % 4
```

can cause **many users to get mapped to different servers**.

With consistent hashing, adding S4 causes only a relatively small portion of keys to move.

```text
Before:

       S1
   ↗       ↘
 S3         S2


After adding S4:

       S1
   ↗       ↘
 S3        S4
   ↖       ↙
       S2
```

Only some keys need to move.

---

## Interview Answer

> **"Yes, a load balancer can use consistent hashing when we want requests associated with the same key, such as user ID or session ID, to consistently reach the same backend server. It is particularly useful for session affinity and distributed caching. However, for general stateless services, simpler algorithms like Round Robin or Least Connections are often sufficient."**

### Important Distinction

Don't confuse:

**Load balancing**

with

**Consistent hashing**

Consistent hashing is primarily a **mapping strategy**:

```text
key → server
```

A load balancer can use that strategy to decide **which backend should receive the request**.

```text
                 Load Balancer
                       │
             Consistent Hashing
                       │
          ┌────────────┼────────────┐
          ↓            ↓            ↓
         S1           S2           S3
```

**Round Robin:**
"Give the next request to the next server."

**Consistent Hashing:**
"Given this key, consistently choose the corresponding server."
