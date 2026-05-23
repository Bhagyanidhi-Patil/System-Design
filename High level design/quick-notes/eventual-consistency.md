# 🔄 Eventual Consistency

## Definition

Eventual consistency means:

> All nodes in a distributed system will become consistent eventually, but not immediately.

For a short time, different servers may have different data, but after synchronization, all servers will contain the same updated data.

---

## 🧠 What are Nodes?

In distributed systems, nodes generally mean:

- Servers  
- Databases  
- Machines/computers  
- Instances/services participating in the system  

A node is basically:

> Any individual machine or component that stores/processes data in a distributed system.

---

## 💻 In Distributed Systems

Suppose:

- Data is stored on multiple servers  
- User updates profile photo on Server A  

### Immediately:
- Server A has new photo  
- Server B may still have old photo  

### After some time:
- All servers sync  
- Every server shows the new photo  

---

## ✅ Characteristics

- Data may be temporarily outdated  
- System remains highly available  
- Synchronization happens in the background  
- Common in distributed databases and cloud systems  

---

## ⚖️ Why Use Eventual Consistency?

It helps systems:

- Stay available  
- Scale easily  
- Continue working during network partitions  

### Trade-off:
- High availability ✅  
- Immediate consistency ❌  

---

## ⚖️ Strong Consistency vs Availability

In distributed systems, strong consistency can reduce availability because the system may need to block requests during network partitions to ensure all nodes return the latest data.

---

## 🔐 Strong Consistency

Every read gets the latest write.

So if data is being updated:
- System must make sure ALL nodes agree before responding

👉 This often requires waiting for network communication.

---

## 🌐 Availability

System always responds, even if data is not fully updated.

So:
- System replies immediately  
- Even if some nodes are out of sync  

---

## 💥 The Conflict (Important Insight)

In a network partition (nodes can’t talk to each other):

### If you choose Strong Consistency (CP system)
- System may block requests  
- Wait until all nodes sync  
- ❌ Some requests fail or timeout  

👉 So availability drops  

---

### If you choose Availability (AP system)
- System responds immediately  
- May return old data  
- ❌ Consistency is temporarily broken  

---

## 🌍 Real-world Intuition (ATM Example)

### Strong Consistency:
- Must show correct balance everywhere  
- If network is down → ATM may stop transactions  

### High Availability:
- ATM always works  
- But balance might be slightly outdated  