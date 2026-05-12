# 🧩 What is a Logger Rate Limiter?

A Logger Rate Limiter is a system component that controls how frequently log messages are printed or recorded.

In real-world systems, logs can be extremely high-volume (especially in distributed systems, APIs, or servers) if they are not handled properly.  
Uncontrolled logging increases infrastructure cost, degrades system performance, and reduces observability.

So, a logger rate limiter ensures that:
❗ The same log message is not printed more than once within a fixed time window.

---
## Note 
logging absolutely consumes disk space, and in many real systems it’s one of the primary hidden costs of observability.

🧾 How logging uses disk space

Every time an application writes a log, it is typically:

Appended to a file on disk (local logs), or
Sent to a logging agent that buffers and stores it temporarily, or
Stored in a centralized logging system that ultimately persists it on disk/object storage

So each log line becomes persistent data, 
for example:

2026-05-12 10:01:03 ERROR Payment failed for user 123

Even a single line like this takes space (bytes → KB → MB → GB over time).
---
## When logs are high-volume and uncontrolled, they become expensive in 4 ways:
## 🧾 1. Storage cost (direct money cost)

Logs are written to:

- disk  
- cloud storage (S3, GCS, etc.)  
- logging platforms (Datadog, Splunk, ELK)

If logs are not controlled:

- terabytes of unnecessary logs accumulate  
- you pay for storage continuously  

👉 This is the literal monetary cost  

---

## ⚡ 2. CPU + I/O cost (performance cost)

Every log line:

- allocates memory  
- formats strings  
- writes to disk/network  
- may serialize JSON  

If logging is excessive:

- CPU usage increases  
- I/O becomes bottleneck  
- application slows down  

👉 This is often the hidden performance killer  

---

## 🌐 3. Network cost (distributed systems)

In microservices:

- logs are shipped to central systems (Kafka / Logstash / Cloud logging)

High log volume means:

- more network bandwidth  
- higher ingestion cost  
- potential backlog in pipelines  

---

## 🔍 4. Signal-to-noise problem (operational cost)

Too many logs cause:

- Important logs get buried  
- Debugging becomes harder  
- On-call engineers waste time  

👉 This is the “human cost”  

---

## Example

If the rule is: “Print a message at most once every 10 seconds”

| Time | Log Request | Printed? |
|------|------------|----------|
| 0s   | "Error A"  | ✅ Yes    |
| 3s   | "Error A"  | ❌ No     |
| 9s   | "Error A"  | ❌ No     |
| 11s  | "Error A"  | ✅ Yes    |

---

## 🧩 Why Logger Rate Limiter exists

It solves:

Instead of:
Error A
Error A
Error A
Error A (1000 times)

You get:
Error A (once every 10 seconds)


---

## 📌 Requirements (LLD)

### Functional Requirements

- Log messages with a timestamp.  
- Suppress duplicate messages within a fixed time window.  
- Allow different messages independently (each message has its own timer).  
- Thread-safe (optional but important in real systems).  

---

## 🧠 Core Idea (Design Approach)

We maintain a hash map:
message → lastPrintedTimestamp
When a log request comes:

- If message is new → print it  
- If message exists:  
  - Check currentTime - lastPrintedTime >= window  
    - Yes → print  
    - No → skip  

---

## 🏗️ Design Principles Used

### 1. Single Responsibility Principle (SRP)

- Logger only handles logging logic  
- No business logic mixed in  

---

### 2. O(1) Lookup using HashMap

- Efficient time-based filtering  

---

### 3. Encapsulation

- Internal state (timestamp map) is hidden  

---

### 4. Open/Closed Principle (OCP)

- Can extend to multi-level loggers (INFO/WARN/ERROR) without modifying core logic  

---

### 5. Thread Safety (optional enhancement)

- Can use mutex for concurrent environments  