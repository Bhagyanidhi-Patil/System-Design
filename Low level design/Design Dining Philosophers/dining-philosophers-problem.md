# Dining Philosophers Problem

## Problem Statement

There are **N philosophers** sitting around a circular table.

Each philosopher alternates between:

* Thinking 🤔
* Eating 🍝

Between every two philosophers is **one fork**.

To eat, a philosopher needs **both the left and right forks**.

```text
         Fork0
     P0 ------- P1
   /              \
Fork4            Fork1
 /                  \
P4                  P2
 \                  /
 Fork3           Fork2
     \          /
          P3
```

For philosopher `i`:

```text
Left Fork  = i
Right Fork = (i + 1) % N
```

---

## Constraints

* One fork can be held by only one philosopher.
* No two neighbouring philosophers can eat together.
* Every philosopher should eventually get a chance to eat.

---

## Challenges

There are three major synchronization problems.

### 1. Deadlock

Suppose everyone picks the left fork first.

```text
P0 picks Fork0

P1 picks Fork1

P2 picks Fork2

P3 picks Fork3

P4 picks Fork4
```

Now everyone waits for the right fork.

```text
P0 waiting Fork1

P1 waiting Fork2

P2 waiting Fork3

P3 waiting Fork4

P4 waiting Fork0
```

Nobody releases.

```text
Forever waiting...
```

This results in **Deadlock**.

---

### 2. Starvation

Suppose philosopher **P3** is very unlucky.

```text
P2 eats

P4 eats

Again P2 eats

Again P4 eats

P3 never gets both forks
```

Philosopher **P3** waits forever and **starves**.

---

### 3. Race Condition

Two philosophers try to grab the same fork simultaneously.

Without synchronization:

```text
Fork2

P1 grabs

P2 grabs

Both think they own it
```

This is incorrect because a fork can be held by only one philosopher at a time.

---

# Low-Level Design (LLD)

## Classes

### DiningTable

```text
DiningTable
-----------------
vector<Fork> forks
vector<Philosopher> philosophers

startDinner()
```

### Fork

```text
Fork
----------------
id
mutex
```

### Philosopher

```text
Philosopher
----------------
id
leftFork
rightFork

think()

eat()

run()
```

---

## Relationships

```text
DiningTable

    |
    | has
    |

+-------------+
|    Fork     |
+-------------+

    |
    | shared by
    |

+----------------+
| Philosopher    |
+----------------+
```

## Complete Class Diagram
```
                 DiningTable
        +---------------------------+
        | philosophers              |
        | forks                     |
        +------------+--------------+
                     |
         -----------------------------
         |                           |
      owns N                      owns N
         |                           |
      +------+                   +----------+
      | Fork |<----------------> |Philosopher|
      +------+     shared        +-----------+
      | id   |                   | id        |
      | mtx  |                   | leftFork  |
      +------+                   | rightFork |
                                 | think()   |
                                 | eat()     |
                                 | run()     |
                                 +-----------+
```