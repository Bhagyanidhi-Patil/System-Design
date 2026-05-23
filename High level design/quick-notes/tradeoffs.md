# ⚖️ Trade-offs

## Definition
In software systems, improving one thing often makes another thing worse, more expensive, or more complex.
A trade-off is a compromise where improving one aspect of a system may negatively affect another aspect.
A trade-off means:

> Gaining one advantage while sacrificing another.


---

## 📌 Simple Real-Life Example

Suppose you buy a laptop:

- High performance laptop → expensive 🔥
- Cheap laptop → lower performance 💸

You cannot maximize both at the same time.

👉 Choosing one benefit over another is called a trade-off.

---

# 💻 Trade-offs in Software Systems

## 1️⃣ Speed vs Cost

- Faster systems need better servers
- Better servers cost more

### 📌 Example
Using powerful cloud servers improves performance but increases cost.

---

## 2️⃣ Consistency vs Availability

*(Distributed Systems)*

- Strong consistency → latest accurate data
- High availability → system always responds

Sometimes you sacrifice one for the other.

---

## 3️⃣ Security vs Convenience

- More security → OTP, MFA, stricter rules
- More convenience → easier login

More security can reduce user convenience.

---

## 4️⃣ Scalability vs Complexity

- Horizontally scalable systems handle more traffic
- But architecture becomes more complex

---

## 🌍 Real-World Example

Using caching:

- ✅ Faster response times
- ❌ May return stale/old data

That is a trade-off between:
- Performance
- Data freshness