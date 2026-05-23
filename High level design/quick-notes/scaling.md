# 📈 Scaling

## Definition

Scaling is the process of increasing a system’s capacity to handle more traffic, users, or data without affecting performance.

---

# Types of Scaling

## 1️⃣ Vertical Scaling (Scale Up)

Increasing the power of a single server by adding:
- More CPU
- More RAM
- More storage

### 📌 Example
Upgrading a server from:
- 8 GB RAM → 32 GB RAM

### ✅ Pros
- Simple to implement
- No major architecture changes

### ❌ Cons
- Hardware limit exists
- Expensive
- Single point of failure

---

## 2️⃣ Horizontal Scaling (Scale Out)

Adding multiple servers and distributing traffic among them.

### 📌 Example
Instead of 1 server:
- Use 5 servers behind a load balancer

### ✅ Pros
- Better fault tolerance
- Handles massive traffic
- Highly scalable

### ❌ Cons
- More complex architecture
- Requires load balancing and distributed systems