# Multi-Tier Cache Flow

## Architecture
```
             MultiTierCache
                   |
          get(key) / put(key)
                   |
       ┌───────────▼───────────┐
       │       L1 - RAM        │
       │       LRUCache        │
       └───────────┬───────────┘
                   │ miss
       ┌───────────▼───────────┐
       │       L2 - SSD        │
       │       LRUCache        │
       └───────────┬───────────┘
                   │ miss
       ┌───────────▼───────────┐
       │       L3 - HDD        │
       │       LRUCache        │
       └───────────────────────┘

```

Each tier is an independent LRU cache. It uses a `hash map` for key lookup:

`unordered_map<int, Node*> cache;`

and a `doubly linked list` to maintain LRU ordering.

Conceptually, the cache manager contains three tier caches:

```text
MultiTierCache
      |
      ├── RAM cache
      ├── SSD cache
      └── HDD cache
```

The main responsibility of `MultiTierCache` is coordinating lookups, promotions, and writes across tiers.

### Tier capacities

- RAM (L1): `2`
- SSD (L2): `3`
- HDD (L3): `5`

Each tier applies its own LRU policy.

## 1. `put(key, value)` — Insert or update

The write policy in this example writes the value to all tiers:
```text
                 put(key, value)
                        |
          +-------------+-------------+
          |             |             |
          v             v             v
     +---------+   +---------+   +---------+
     | RAM L1  |   | SSD L2  |   | HDD L3  |
     +---------+   +---------+   +---------+
          |             |             |
          v             v             v
     Insert/Update Insert/Update Insert/Update
          |             |             |
          v             v             v
     LRU eviction  LRU eviction  LRU eviction
     if full       if full       if full
```

- Write the value to all tiers.
- Update existing keys and move them to the most recently used (MRU) position.
- If a tier is full, evict that tier's least recently used (LRU) item independently.

---

## 2. `get(key)` — Lookup and promotion

```text
                  get(key)
                     |
                     v
              +-------------+
              |  Check RAM  |
              +-------------+
                     |
              Hit? --+-- No
               |           |
              Yes          v
               |     +-------------+
               |     |  Check SSD  |
               |     +-------------+
               |           |
               |     Hit? --+-- No
               |      |          |
               |     Yes         v
               |      |    +-------------+
               |      |    |  Check HDD  |
               |      |    +-------------+
               |      |          |
               |      |     Hit? | No
               |      |      |   |
               |      |      |   v
               |      |      | Return -1
               |      |      |
               +------+------+
                      |
                      v
                Return value
                      |
         +------------+------------+
         |            |            |
      RAM hit      SSD hit      HDD hit
         |            |            |
         v            v            v
     No promotion  Copy to RAM  Copy to RAM
                               and SSD
```

- Search in order: RAM → SSD → HDD.
- On a hit, update that tier's LRU order.
- On a slower-tier hit, copy the value into all faster tiers.
- Promotion does not delete the lower-tier copies.
- If all tiers miss, return `-1`.

---

## 3. `erase(key)` — Delete from all tiers

```text
                  erase(key)
                      |
          +-----------+-----------+
          |           |           |
          v           v           v
     +---------+ +---------+ +---------+
     | RAM L1  | | SSD L2  | | HDD L3  |
     +---------+ +---------+ +---------+
          |           |           |
          v           v           v
     Find node   Find node   Find node
          |           |           |
          v           v           v
     Remove DLL  Remove DLL  Remove DLL
     node        node        node
          |           |           |
          v           v           v
     Erase map   Erase map   Erase map
     entry       entry       entry
          |           |           |
          v           v           v
     Delete node Delete node Delete node
```

- Remove the key from every tier.
- Update both the hash map and doubly linked list in each tier.
- Unlike LRU eviction, this explicitly deletes the key across all tiers.



## Multi-Tier Cache — Example Walkthrough

### Initial Setup

- RAM capacity = 2
- SSD capacity = 3
- HDD capacity = 5
- Order: MRU (front) → LRU (back)

### 1. PUT Operations

### Operation: `cache.put(1, 100)`

Insert key 1 with value 100 into every tier.

```text
RAM:  [(1,100)]
SSD:  [(1,100)]
HDD:  [(1,100)]
```

### Operation: `cache.put(2, 200)`

Insert key 2 with value 200 into every tier.

```text
RAM:  [(2,200) → (1,100)]
SSD:  [(2,200) → (1,100)]
HDD:  [(2,200) → (1,100)]
```

### Operation: `cache.put(3, 300)`

Insert key 3 into every tier.

RAM is full, so it evicts key 1 (its LRU item).
SSD and HDD have sufficient capacity.

```text
RAM:  [(3,300) → (2,200)]
SSD:  [(3,300) → (2,200) → (1,100)]
HDD:  [(3,300) → (2,200) → (1,100)]
```

### 2. GET and Promotion

### Operation: `cache.get(1)`

**Step 1:** Search RAM → Miss.

**Step 2:** Search SSD → Hit! Value = 100.

**Step 3:** Promote key 1 into RAM.

RAM is full, so it evicts key 2 (the LRU item).

```text
RAM:  [(1,100) → (3,300)]
SSD:  [(1,100) → (3,300) → (2,200)]
HDD:  [(3,300) → (2,200) → (1,100)]
```

Returned value: `100`

Note: SSD's LRU order changes because it was accessed. HDD is not accessed, so its order stays unchanged.

### Operation: `cache.get(2)`

**Step 1:** Search RAM → Miss.

**Step 2:** Search SSD → Hit! Value = 200.

**Step 3:** Promote key 2 into RAM.

RAM evicts key 3, its current LRU item.

```text
RAM:  [(2,200) → (1,100)]
SSD:  [(2,200) → (1,100) → (3,300)]
HDD:  [(3,300) → (2,200) → (1,100)]
```

Returned value: `200`

### 3. UPDATE Operation

### Operation: `cache.put(1, 500)`

Update key 1 to value 500 in every tier.

```text
RAM:  [(1,500) → (2,200)]
SSD:  [(1,500) → (2,200) → (3,300)]
HDD:  [(1,500) → (3,300) → (2,200)]
```

All existing copies now have value 500 for key 1.

### 4. DELETE Operation

### Operation: `cache.erase(2)`

Delete key 2 from RAM, SSD, and HDD.

```text
RAM:  [(1,500)]
SSD:  [(1,500) → (3,300)]
HDD:  [(1,500) → (3,300)]
```

Key 2 no longer exists in any tier.

### 5. Cache Miss

### Operation: `cache.get(4)`

```text
RAM → Miss
SSD → Miss
HDD → Miss
```

Returned value: `-1`


