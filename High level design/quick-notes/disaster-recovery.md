# Disaster Recovery (DR) in HLD

**Disaster Recovery (DR)** is the process of restoring a system after a catastrophic failure (for example, an entire data center outage, cloud region failure, accidental data deletion, or ransomware attack).

The goal is **not to prevent failures**, but to **recover quickly with minimal data loss and downtime**.

---

# When Do We Need Disaster Recovery?

Disaster Recovery is required when the **entire production environment becomes unavailable**, such as:

* Entire database cluster crashes
* Cloud region outage
* Data center power failure
* Flood, fire, earthquake
* Accidental database deletion
* Data corruption
* Cyber attacks (Ransomware)

### Example

```text id="gruexu"
                Region A

          Primary Database ❌

          Read Replica ❌

          Cache ❌

          Servers ❌
```

The whole production environment is down.

---

# Goal of Disaster Recovery

There are two important metrics every interviewer expects you to know.

## 1. RTO (Recovery Time Objective)

**How quickly should the system recover?**

Example:

```text id="48dcur"
RTO = 30 minutes
```

Meaning:

> The application should be back online within **30 minutes**.

---

## 2. RPO (Recovery Point Objective)

**How much data loss is acceptable?**

Example:

```text id="4vsv5a"
RPO = 5 minutes
```

If the disaster happens at:

```text id="oev0ud"
10:30 AM
```

Latest backup:

```text id="966yb8"
10:25 AM
```

Maximum acceptable data loss:

```text id="uo8ski"
5 minutes
```

### Interview Tip

Remember:

```text id="amp6up"
RTO → Time to Recover

RPO → Maximum Data Loss Allowed
```

---

# Disaster Recovery Architecture

```text id="4n9zmp"
                    Users
                      |
                Load Balancer
                      |
                 Region A (Primary)
              ---------------------
              |                   |
          Application         Database
              |                   |
              ---------------------
                      |
          Continuous Replication
                      |
                 Region B (Standby)
              ---------------------
              |                   |
          Application         Database
```

Normally,

* All traffic goes to **Region A**.
* **Region B** remains ready as a standby environment.

---

# What Happens During a Disaster?

Suppose **Region A** fails.

```text id="vbc269"
Region A ❌
```

Traffic automatically switches to:

```text id="egatux"
Region B
```

Users continue using the application with minimal interruption.

---

# Types of Disaster Recovery

## 1. Backup & Restore

This is the simplest Disaster Recovery strategy.

Take periodic backups.

Example:

```text id="o28k2n"
Every Day

↓

Backup Database

↓

Store Backup in S3
```

If the database crashes:

```text id="xlp00g"
Restore Backup

↓

Start Database
```

### Advantages

* Cheapest solution
* Easy to implement

### Disadvantages

* Recovery is slow
* Recent data may be lost

Example:

```text id="tieosk"
RTO = Hours

RPO = Several Hours
```

---

## 2. Warm Standby

A smaller version of the production environment continuously runs.

```text id="o2yjnv"
Production

↓

Replication

↓

Smaller Standby
```

During a disaster:

```text id="ydyme3"
Scale Standby

↓

Serve Traffic
```

Recovery is much faster than Backup & Restore.

---

## 3. Hot Standby (Active-Passive)

Both production and standby environments are continuously running.

```text id="mqce20"
Region A (Primary)

↓

Real-Time Replication

↓

Region B (Standby)
```

If Region A fails,

Traffic immediately switches to Region B.

### Advantages

* Very low downtime
* Very low data loss

### Disadvantage

* Expensive to maintain

---

## 4. Active-Active

Both regions serve users simultaneously.

```text id="jh44fo"
          Users
            |
      Load Balancer
       /          \
 Region A      Region B
```

If Region A fails,

Users continue using Region B.

### Advantages

* Almost zero downtime
* Very high availability

### Disadvantage

* Most expensive architecture
* Complex data synchronization

---

# Database Backup Types

## 1. Full Backup

Copies the entire database.

```text id="c3e27j"
Database

↓

Complete Backup
```

### Pros

* Easy to restore

### Cons

* Requires more storage
* Takes longer to complete

---

## 2. Incremental Backup

Stores only the changes made since the previous backup.

Example:

```text id="c9wprj"
Monday

Full Backup

Tuesday

Changed Data Only

Wednesday

Changed Data Only
```

### Advantages

* Fast backup
* Less storage required

---

## 3. Differential Backup

Stores all changes made since the last full backup.

```text id="jjo076"
Monday

Full Backup

Tuesday

Changes

Wednesday

Tuesday + Wednesday Changes
```

Restoration is faster than incremental backups because fewer backup files are required.

---

# Point-in-Time Recovery (PITR)

## Why Do We Need Point-in-Time Recovery (PITR)?

Imagine your database takes a **full backup every night at 12:00 AM**.

```text id="gt9dup"
12:00 AM -------- Full Backup -------->
```

Now your application is running normally.

Users continue performing operations.

```text id="jhkkx8"
12:10  User A registers

12:20  User B places an order

12:35  User C makes payment

12:45  Admin updates prices

1:00   Someone accidentally deletes the database
```

---

## What Happens If We Only Have Backups?

The latest backup is:

```text id="9iz8qd"
12:00 AM
```

The database crashes at:

```text id="539zts"
1:00 AM
```

If we restore the backup,

```text id="ny1n5z"
Restore

↓

12:00 AM Database
```

Everything that happened between:

```text id="ec63he"
12:00 AM → 1:00 AM
```

is lost forever.

That means you lose:

* New users
* Orders
* Payments
* Messages
* Likes
* Comments

This results in **1 hour of data loss**.

This is exactly why **backups alone are not enough**.

---

## How Does PITR Solve This?

- Instead of storing only periodic backups, the database also records **every change** made after the backup.
- These changes are stored in **transaction logs**.
- After a backup is taken, the database continues writing new transaction logs. During recovery, you restore the backup first, then replay only the logs generated after that backup.
- However, databases do not necessarily delete old logs immediately. They retain them until they're no longer needed for recovery, replication, or backup policies.

```text id="enc5hl"
12:00 AM

Full Backup
```

Then every database operation is recorded.

```text id="836rc3"
12:10

INSERT User
```

```text id="gzp1as"
12:20

INSERT Order
```

```text id="uqoyek"
12:35

UPDATE Payment
```

```text id="j7w7ro"
12:45

DELETE Product
```

These transaction logs are written continuously as users interact with the application.

---

## What Are Transaction Logs?

Think of transaction logs as a **diary of every database change**.

Instead of storing the entire database again, the database stores only:

> **"What changed?"**

For example,

Instead of storing:

```text id="gfw467"
Users Table

1 Million Rows
```

it simply records the operation:

```sql id="yu6ki7"
INSERT INTO users
VALUES (101, 'Bhoomika');
```

Next operation:

```sql id="sulkgt"
UPDATE Orders
SET status = 'Delivered'
WHERE id = 20;
```

Next operation:

```sql id="yqxwv5"
DELETE
FROM Cart
WHERE id = 45;
```

Every change is recorded sequentially in the exact order in which it occurred.

---

## Recovery Using PITR

Suppose:

```text id="nx1ig7"
12:00 AM

Full Backup
```

The database crashes at:

```text id="hse7qb"
1:00 AM
```

`Recovery consists of two steps.`

---

### Step 1: Restore the Latest Backup

Restore the full backup taken at 12:00 AM.

```text id="kjigm6"
12:00 AM Database
```

At this point, the database is back to its 12:00 AM state.

---

### Step 2: Replay Transaction Logs

Replay every transaction log generated after the backup.

```text id="8lje0v"
12:10

INSERT User
```

↓

```text id="kudrnv"
12:20

INSERT Order
```

↓

```text id="69kalb"
12:35

UPDATE Payment
```

↓

```text id="mr5vs3"
12:45

DELETE Product
```

↓

```text id="0hsenm"
Continue replaying logs...

Until 12:59:59
```

The recovered database now becomes:

```text id="r91agk"
Exactly how the database looked

at 12:59:59
```

As a result, **almost no data is lost**, even though the last full backup was taken an hour earlier.


---

# Disaster Recovery Process

Suppose the production environment crashes.

## Step 1: Detect Failure

```text id="9f57a2"
Monitoring

↓

Alert
```

---

## Step 2: Declare Disaster

```text id="dwjg4c"
Primary Region

Unavailable
```

---

## Step 3: Switch Traffic

```text id="26k31t"
Load Balancer

↓

Standby Region
```

---

## Step 4: Restore Services

```text id="l7fyem"
Application

Database

Cache
```

---

## Step 5: Verify System Health

```text id="t9ywav"
Health Checks

↓

Accept Traffic
```

Ensure all services are healthy before routing full production traffic to the standby environment.
