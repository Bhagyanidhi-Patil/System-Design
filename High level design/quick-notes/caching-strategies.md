# 🧠 What is Caching?

Caching means:

Storing frequently used data in a fast storage layer so it can be accessed quickly without going to the main database every time.

Example:

Instead of asking the database again and again, we store result in cache.

---

# 🚀 Why caching is used?

- Faster response time  
- Reduces database load  
- Improves scalability  
- Reduces cost  

---

# 🔄 Main Caching Strategies

## 1️⃣ Cache-Aside (Lazy Loading) ⭐ (Most common)

How it works:
- App first checks cache  
- If data is not there → go to DB  
- Store result in cache  
- Next time → serve from cache  

Flow:
Client → Cache → (miss) → Database → Cache → Client  

👍 Pros  
- Simple  
- Only needed data is cached  

👎 Cons  
- First request is slow (cache miss)  

---

## 2️⃣ Write-Through Cache

How it works:
- Every write goes to cache AND database at the same time  

Flow:
Client → Cache → DB (both updated)  

👍 Pros  
- Cache always updated  
- Strong consistency  

👎 Cons  
- Slower writes  

---

## 3️⃣ Write-Back (Write-Behind) Cache

How it works:
- Write only to cache first  
- DB is updated later asynchronously  

Flow:
Client → Cache → (later) DB  

👍 Pros  
- Very fast writes  

👎 Cons  
- Risk of data loss if cache fails  

---

## 4️⃣ Write-Around Cache

How it works:
- Write directly to DB  
- Cache is not updated immediately  
- Cache updates only on read  

Flow:
Write → DB only  
Read → Cache (miss → DB → cache)  

👍 Pros  
- Avoids unnecessary cache pollution  

👎 Cons  
- First read is slow  