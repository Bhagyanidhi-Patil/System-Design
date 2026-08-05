# RAID (Redundant Array of Independent Disks)

RAID (Redundant Array of Independent Disks) is a technique of combining multiple physical disks into one logical storage unit to achieve one or more of these goals:

* ✅ Higher Performance
* ✅ Higher Availability (Fault Tolerance)
* ✅ Larger Storage Capacity

Instead of storing everything on one disk, RAID distributes data across multiple disks.

Imagine you have **4 hard disks of 1TB each**.

### Without RAID

```text
Disk1 = 1TB
Disk2 = 1TB
Disk3 = 1TB
Disk4 = 1TB

Independent disks
```

### With RAID

```text
        RAID Controller

             |
   ---------------------
   |    |    |     |
 Disk1 Disk2 Disk3 Disk4

Looks like ONE storage volume
```

Applications don't know multiple disks exist.

---

## Why do we need RAID?

Suppose your database is running on a single disk.

```text
Database
    |
 SSD (1TB)
```

If the SSD crashes...

```text
Database ❌
Entire data gone
```

This results in complete data loss.

Now imagine another problem.

One SSD can read:

```text
500 MB/sec
```

But your application requires:

```text
2000 MB/sec
```

A single disk cannot meet this demand.

RAID solves both problems by improving performance and providing fault tolerance.

---

## RAID provides

### 1. Performance

Multiple disks work together.

Instead of:

```text
Read 1GB

Single disk
↓

1GB
```

RAID distributes the workload:

```text
Disk1 → 250MB
Disk2 → 250MB
Disk3 → 250MB
Disk4 → 250MB

Parallel read
```

This significantly improves read and write speed.

---

### 2. Fault Tolerance

If one disk fails:

```text
Disk1 ❌

Data still available
```

The data can still be recovered because another disk contains either a copy (mirroring) or parity information.

---

### 3. Larger Capacity

Instead of using disks independently:

```text
1TB
1TB
1TB
1TB
```

RAID combines them into a single logical volume:

```text
4TB logical storage
```

(The actual usable capacity depends on the RAID level.)

---

## Three Important Concepts

Everything in RAID is based on two core ideas.

### 1. Striping

Striping means **splitting data across multiple disks**.

Example:

```text
Data:

ABCDEFGH
```

Instead of storing:

```text
Disk1

ABCDEFGH
```

Store:

```text
Disk1 : AB
Disk2 : CD
Disk3 : EF
Disk4 : GH
```

All disks can read simultaneously, greatly improving performance.

---

### 2. Mirroring

Mirroring means storing an exact copy of the data.

Example:

```text
Disk1

ABCDEFG
```

Copy:

```text
Disk2

ABCDEFG
```

If Disk1 fails:

```text
Use Disk2
```

No data is lost.

---

### 3. Parity

Parity stores additional information that allows missing data to be reconstructed if a disk fails.

Example:

```text
A = 5
B = 7
```

Parity is calculated using XOR:

```text
P = A XOR B
```

If the disk containing A fails:

```text
A = P XOR B
```

Thus, RAID can recover lost data without storing a complete duplicate.

Parity uses less storage than mirroring but makes writes slower because parity must be updated.

---

# RAID Types

The RAID levels most commonly asked in interviews are:

* RAID 0
* RAID 1
* RAID 5
* RAID 6
* RAID 10 (1+0)

---

## RAID 0 (Striping Only)

## Idea

* Only striping
* No backup
* No parity
* No mirroring

### Example

```text
Data = ABCD

Disk1 → AB
Disk2 → CD
```

Both disks work in parallel.

### Read

```text
Disk1 reads AB
Disk2 reads CD

Parallel
```

Very fast.

### Write

Writes also happen in parallel.

Fast.

### Storage

```text
2 disks

1TB each

Total = 2TB
```

100% usable capacity.

### Failure

If:

```text
Disk2 dies
```

Then:

```text
AB available

CD gone
```

The entire file becomes unusable.

### RAID 0 Summary

```text
Performance      ⭐⭐⭐⭐⭐
Fault Tolerance  ❌
Storage          ⭐⭐⭐⭐⭐
```

**Use Case:** Temporary files, cache, or workloads where maximum speed is needed and data can be recreated.

---

## RAID 1 (Mirroring)

Stores identical copies of the data.

```text
Disk1

ABCDEFG

Disk2

ABCDEFG
```

### Read

Reads can happen from either disk.

### Write

Writes must occur on both disks.

Slightly slower than RAID 0.

### Capacity

```text
2 × 1TB

Usable

1TB
```

50% storage efficiency.

### Failure

```text
Disk1 crashes

↓

Disk2 still has everything
```

No data loss.

### RAID 1 Summary

```text
Performance      Good
Fault Tolerance  Excellent
Storage          50%
```

**Use Case:** Operating system drives and critical small datasets.

---

## RAID 5 (Striping + Distributed Parity)

Most common enterprise RAID level.

Requires at least **3 disks**.

Example:

```text
Disk1   A
Disk2   B
Disk3   Parity
```

Parity rotates across disks:

```text
Disk1   Parity
Disk2   C
Disk3   D
```

Then:

```text
Disk1   E
Disk2   Parity
Disk3   F
```

This prevents any single disk from becoming a bottleneck.

### Capacity

```text
4 disks

1TB each

Usable

3TB
```

One disk's worth of capacity is used for parity.

### Failure

If Disk2 fails:

```text
Disk1   A

Disk2   ❌

Disk3   Parity
```

The missing data is reconstructed using parity.

### Performance

* Reads: Very good
* Writes: Slower due to parity calculations

### RAID 5 Summary

```text
Performance      Good
Fault Tolerance  One disk failure
Storage          N-1 disks
```

**Use Case:** File servers requiring a balance between performance, storage efficiency, and fault tolerance.

---

## RAID 6 (Double Parity)

Similar to RAID 5 but stores **two parity values**.

Requires at least **4 disks**.

### Capacity

```text
4 disks

Usable

2TB
```

Two disks' worth of capacity is reserved for parity.

### Failure

Can survive:

```text
Disk1 ❌

Disk3 ❌
```

Still operational.

### Performance

* Reads: Good
* Writes: Slower than RAID 5 because two parity blocks must be updated.

### RAID 6 Summary

```text
Performance      Good Read
Fault Tolerance  Two disk failures
Storage          N-2 disks
```

**Use Case:** Large storage arrays where higher reliability is required.

---

## RAID 10 (RAID 1 + RAID 0)

Combines mirroring and striping.

Requires at least **4 disks**.

### Step 1: Mirroring

```text
Disk1 ←→ Disk2

Disk3 ←→ Disk4
```

### Step 2: Striping

```text
Pair1 stores

AB

Pair2 stores

CD
```

### Capacity

```text
4 disks

1TB each

Usable

2TB
```

### Failure

Can survive multiple failures **provided both disks in the same mirrored pair do not fail**.

Example:

```text
Disk1 ❌

Disk4 ❌

Still works
```

But:

```text
Disk1 ❌

Disk2 ❌

Same mirror lost

Array fails
```

### Performance

* Excellent reads
* Excellent writes
* Excellent availability

### RAID 10 Summary

```text
Performance      ⭐⭐⭐⭐⭐
Fault Tolerance  ⭐⭐⭐⭐⭐
Storage          50%
```

**Use Case:** High-performance databases and transactional systems.

---

## RAID Comparison

| RAID    | Minimum Disks | Speed                      | Fault Tolerance                                 | Usable Capacity |
| ------- | ------------: | -------------------------- | ----------------------------------------------- | --------------: |
| RAID 0  |             2 | ⭐⭐⭐⭐⭐                      | None                                            |               N |
| RAID 1  |             2 | ⭐⭐⭐                        | 1 disk per mirror                               |             N/2 |
| RAID 5  |             3 | ⭐⭐⭐⭐ (reads), ⭐⭐⭐ (writes) | 1 disk                                          |             N−1 |
| RAID 6  |             4 | ⭐⭐⭐⭐ (reads), ⭐⭐ (writes)  | 2 disks                                         |             N−2 |
| RAID 10 |             4 | ⭐⭐⭐⭐⭐                      | Multiple failures (not in the same mirror pair) |             N/2 |

---

## Which RAID is used where?

| Use Case                                               | Recommended RAID | Reason                                           |
| ------------------------------------------------------ | ---------------- | ------------------------------------------------ |
| Temporary files / Cache                                | RAID 0           | Maximum performance                              |
| Operating System Drives                                | RAID 1           | Simple redundancy                                |
| File Servers                                           | RAID 5           | Good balance of capacity and reliability         |
| Large Storage Arrays                                   | RAID 6           | Better protection against multiple failures      |
| High-Performance Databases (MySQL, PostgreSQL, Oracle) | RAID 10          | Fast reads/writes with excellent fault tolerance |


## Easy Way to Remember RAID Levels

* **RAID 0** → **0 protection**, only speed (Striping only).
* **RAID 1** → **1 copy (Mirror)** of your data (Mirroring).
* **RAID 5** → **Striping + 1 Parity**, survives **1 disk failure**.
* **RAID 6** → **Striping + 2 Parities**, survives **2 disk failures**.
* **RAID 10 (1+0)** → **RAID 1 (Mirroring) + RAID 0 (Striping)**, providing both high performance and fault tolerance. It is one of the most common choices for performance-critical databases such as MySQL, PostgreSQL, and Oracle.
