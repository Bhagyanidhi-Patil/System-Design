### Question : In whatsapp, the stickers you use recently appear at top and rarely used stickers slowly dissappera from the list, which data structure and algo is used to design this feature?

A feature like WhatsApp's "recently used stickers" is typically implemented using a combination of:
```
1. Hash Map + Doubly Linked List (LRU-style)
2. Frequency + Recency Ranking
3. Top-K Stickers
```
---

### 1. Hash Map + Doubly Linked List (LRU-style)

This is the same idea behind an **LRU (Least Recently Used) cache**.

**Data structures:**

* **Hash Map**: `stickerId -> node`
* **Doubly Linked List**: stores stickers ordered by recency

**Operations:**

* When a sticker is used:

  * If already in the list, move it to the front.
  * If new, insert at the front.
* If the list exceeds a maximum size (say 100 stickers), remove from the tail.

**Complexity:**

* Access/update: **O(1)**
* Move to front: **O(1)**
* Eviction: **O(1)**

**Example:**

```text
Most recent
[😀] <-> [🎉] <-> [❤️] <-> [👍]
Least recent
```

Using ❤️ again:

```text
[❤️] <-> [😀] <-> [🎉] <-> [👍]

```
---

### 2. Frequency + Recency Ranking (More Likely for WhatsApp)

Your observation is important:

> Rarely used stickers slowly disappear, not immediately.

This suggests WhatsApp may not be using pure LRU. Instead, it probably computes a **score** based on:

```text
score = frequency_weight × usage_count
      + recency_weight × recent_usage
```

or a time-decayed score:

```text
score = usage_count × e^(-age / decay_constant)
```

where:

* `usage_count` = how many times you've used the sticker
* `age` = time since last use

**Examples:**
```
| Sticker | Uses | Last Used   |
| ------- | ---- | ----------- |
| A       | 50   | 30 days ago |
| B       | 5    | 1 hour ago  |
| C       | 20   | 1 day ago   |
```
Computed scores might be:

```text
A = 8
B = 15
C = 25
```

Ordering:

```text
C, B, A
```

This prevents a sticker used once recently from permanently outranking a sticker used hundreds of times.

---

### 3. Top-K Stickers

To display only the first few stickers:

* Store scores in a **max heap (priority queue)**, or
* Periodically sort by score.

**Complexity:**

* Update score: **O(log n)**
* Get top stickers: **O(k log n)** or **O(n log n)** if sorted periodically.


---