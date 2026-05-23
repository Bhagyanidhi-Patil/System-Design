# 🔁 Failover

Failover is a mechanism used in distributed systems to ensure high availability when a system component fails.

👉 In simple terms:  
If one server/database goes down, another one automatically takes over.

---

# 🧠 Definition

Failover = automatic switching to a backup system when the primary system fails

---

# 🧾 Real-world example

Imagine you are using a banking app:

- Primary database server handles all transactions 💳  
- Backup server stays ready in the background 🛟  

If the primary server crashes:

👉 System automatically switches to backup server  
👉 You can still use the app without interruption  

---

# 🔄 How Failover Works

- System continuously monitors primary server (health checks)  
- If failure is detected:  
  - Primary is marked DOWN ❌  
  - Backup server is promoted to PRIMARY ✅  
  - Traffic is redirected automatically  

---

# 🧩 Types of Failover

## 1️⃣ Active–Passive Failover

- One server is active (handles traffic)  
- One server is passive (idle backup)  

### Flow:
- Primary handles requests  
- Backup stays ready  
- On failure → backup takes over  

👍 Simple  
👎 Backup is underutilized  

---

## 2️⃣ Active–Active Failover

- Multiple servers are active at the same time  
- Load is shared  

### Flow:
- Both servers handle traffic  
- If one fails → others continue working  

👍 Better performance + utilization  
👎 More complex  

---

# 🧠 Where Failover is used

- Databases (MySQL, PostgreSQL clusters)  
- Cloud systems (AWS, Azure, GCP)  
- Load balancers  
- Microservices  
- DNS systems  

---

# ⚠️ Why Failover is important

Without failover:

- System downtime ⛔  
- Data loss risk  
- Business impact  
- Poor user experience  

With failover:

- High availability (99.9% uptime)  
- Fault tolerance  
- Seamless user experience  