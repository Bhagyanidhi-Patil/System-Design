
## Multi-Level / Multi-Tier Cache Architecture

### Storage tiers

```text
┌──────────────────────────────────────────────┐
│                  L1 — RAM                    │
│                                              │
│  Speed    : Fastest                          │
│  Capacity : Smallest                         │
│  Cost     : Highest per GB                   │
│  Usage    : Frequently accessed items        │
└──────────────────────────────────────────────┘
                      ↕
                 Promotion /
                  Demotion
                      ↕
┌──────────────────────────────────────────────┐
│                  L2 — SSD                    │
│                                              │
│  Speed    : Medium                           │
│  Capacity : Medium                           │
│  Cost     : Medium per GB                    │
│  Usage    : Recently/moderately used items   │
└──────────────────────────────────────────────┘
                      ↕
                 Promotion /
                  Demotion
                      ↕
┌──────────────────────────────────────────────┐
│                  L3 — HDD                    │
│                                              │
│  Speed    : Slowest                          │
│  Capacity : Largest                          │
│  Cost     : Lowest per GB                    │
│  Usage    : Cold data / long-term cache      │
└──────────────────────────────────────────────┘
```

The application uses two main operations:

- `get(key)` — retrieve a value.
- `put(key, value)` — insert or update a value.

The cache internally decides which storage tier to use.

> **Important:** RAM, SSD, and HDD are storage tiers, not three independent caches. Together, they form one logical cache.

In a multi-tier cache, data can move between tiers according to the cache policy.

### Promotion

Promotion moves an item to a faster tier. When an item is accessed in a slower tier, the cache can copy it into faster tiers.

For example, `get("user:101")` might follow this path:

```text
RAM  → Cache miss
  ↓
SSD  → Cache miss
  ↓
HDD  → Cache hit
  ↓
Copy the item to SSD and RAM
```

The next access can then be served directly from RAM.

### Demotion

Demotion moves an item to a slower tier. When RAM is full, an eviction policy such as LRU selects an item to remove. Depending on the design, that item may be moved to SSD if it should remain cached there.

```text
RAM  → Item evicted
  ↓
SSD  → Item retained or stored
  ↓
HDD  → Optional further demotion
```
