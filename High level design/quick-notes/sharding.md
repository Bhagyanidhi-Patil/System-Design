# 🧩 Partitioning / Sharding

Partitioning (also called sharding in distributed systems) is a way of splitting a large database into smaller pieces so it becomes faster, scalable, and easier to manage.

Instead of keeping all data in one big database, we divide it across multiple databases or servers.

---

# 📦 Why do we need it?

When a database becomes very large:

- Queries become slow  
- One server gets overloaded  
- Scaling becomes expensive  
- Maintenance becomes difficult  

So we split the data.

---

# 🔪 What is Partitioning?

Partitioning = dividing data within the same database system

### Example:
A users table can be split like:

- Users A–M → Partition 1  
- Users N–Z → Partition 2  

Still usually managed under one database system.

---

# 🧱 What is Sharding?

Sharding = partitioning across multiple database servers

Each shard is a separate database instance.

### Example:

- Shard 1 → Users (India)  
- Shard 2 → Users (USA)  
- Shard 3 → Users (Europe)  

Each shard stores only a subset of total data.

---

# 🔄 How Sharding Works

- Application receives request  
- A shard key is used (like `user_id`, region, etc.)  
- System decides which shard contains the data  
- Request goes only to that shard  

### Example:
user_id = 105 → Shard 2


---

# 🧠 Common Sharding Strategies

## 1️⃣ Range-based Sharding

Data is split by range.

Example:

- 1–1000 → Shard 1  
- 1001–2000 → Shard 2  

👍 Simple  
👎 Can cause uneven load (hot shard problem)

---

## 2️⃣ Hash-based Sharding

A hash function decides shard.

Example:
shard = hash(user_id) % 3


👍 Even distribution  
👎 Hard to add/remove shards

---

## 3️⃣ Directory-based Sharding

A lookup service tells where data is stored.

Example:
user_id → Shard mapping table


---

# 🧩 Horizontal Sharding

## 👉 Meaning

Horizontal sharding = splitting data row-wise across multiple databases

Each shard contains the same schema, but different rows of data.

---

## 🧾 Example

A Users table:
user_id name

Instead of storing all users in one DB:

- Shard 1 → user_id 1–1M  
- Shard 2 → user_id 1M–2M  
- Shard 3 → user_id 2M–3M  

Each shard is a separate database instance.

---

## 🔄 How it works

- A shard key (like `user_id`) decides where data goes  
- App routes query to correct shard  
- Each shard handles only a subset of total users  

---

# 🔥 Horizontal Sharding Disadvantage: Hot Shard Problem

In horizontal sharding, data is split across multiple shards (databases). Ideally, traffic is evenly distributed.

But sometimes:

👉 One shard gets much more traffic than others  

That shard becomes a bottleneck.

---

## 🧾 Example

Imagine a social media app:

- Shard 1 → users A–F  
- Shard 2 → users G–M  
- Shard 3 → users N–Z  

Now suppose:

👉 Users in shard 2 include a celebrity or viral region  
👉 Most requests target those users  

So:

- Shard 2 gets huge traffic 🚨  
- Shard 1 and 3 are mostly idle  

---

# 🧱 Vertical Sharding (Less common term)

## 👉 Meaning

Vertical sharding = splitting data by columns or by features

Instead of splitting rows, you split columns or domains.

---

## 🧾 Example

Original table:
| user_id | name | email | password | profile_pic | preferences |


You split it into:

---

### Shard A (User Identity DB)
| user_id | name | email | password |---

### Shard B (User Profile DB)

| user_id | profile_pic | preferences |


## 🔄 How it works

- Each shard stores different attributes of the same entity  
- Joined using `user_id` when needed  

---

## 👍 Advantages

- Reduces table size  
- Improves performance (less data per query)  
- Better security separation (e.g., auth vs profile data)  

---

## 👎 Disadvantages

- Joins across databases are expensive  
- More complex application logic  
- Harder consistency management  