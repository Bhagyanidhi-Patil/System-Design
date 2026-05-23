# 🚧 Bottlenecks

## Definition

A bottleneck is a component in a system that limits overall performance or slows down the entire system.

It happens when one part cannot handle the required load or speed compared to other parts.

---

## 📌 Easy Real-Life Example

Imagine you are filling water into a bottle using a funnel.

- The top part is wide
- But the funnel neck is very narrow

Even if you pour water fast, water can only pass slowly through the narrow neck.

👉 That narrow neck is the bottleneck because it limits speed.

---

## 💻 Bottlenecks in Software Systems

Common bottlenecks include:
- CPU overload
- Low memory (RAM)
- Slow database queries
- Network latency
- Disk I/O limitations
- Too many requests on one server

---

## 🌍 Real-World Example

Suppose:
- Your application can handle 10,000 users
- But your database can only process 1,000 requests/sec

👉 The database becomes the bottleneck because it limits the whole system.

---

## ⚠️ Why Bottlenecks Matter

Bottlenecks can cause:
- Slow response times
- High latency
- System crashes
- Poor scalability
- Bad user experience

---

## ✅ How to Reduce Bottlenecks

- Use caching
- Optimize database queries
- Add load balancers
- Scale horizontally
- Upgrade hardware
- Use asynchronous processing