**Redis is a NoSQL database.**

More specifically, Redis is an **in-memory key-value store.**

Example:

```text
user:123 -> John
rate:user123 -> 45
session:abc -> {...}
```

Unlike SQL databases, Redis doesn't primarily organize data around relational tables, joins, and foreign keys.

## Why is Redis Fast?

Because data is stored in RAM.

Access times are roughly:

```text
RAM      -> nanoseconds
SSD/HDD  -> microseconds to milliseconds
```

So a Redis lookup:

```text
GET user:123
```

can be extremely fast compared to reading from disk.

## Your Question: If Redis Uses RAM, Won't Data Be Lost After Restart?

By default, yes.

If Redis only kept data in memory:

```text
RAM
 |
Redis
```

and the server crashes:

Power failure

then all data would disappear.

That's why Redis provides persistence mechanisms.

### Method 1: RDB Snapshots
_(Redis Database Snapshot)_

Redis periodically saves memory contents to disk.
Think of RDB like taking a photo of Redis memory.

```text
Time 10:00  -> Snapshot
Time 10:05  -> Snapshot
Time 10:10  -> Snapshot
```

Redis writes the entire dataset to disk:

```text
dump.rdb
```

Example:

Every 5 minutes

Redis creates:

```text
dump.rdb
```

on disk.

Flow:

```text
RAM
 |
Snapshot
 |
Disk
```

If Redis crashes:

Restart

it loads the latest snapshot.

Drawback

Data between snapshots can be lost.

Example:

```text
10:00 snapshot saved
10:04 crash
```

You may lose 4 minutes of data.

### Method 2: AOF (Append Only File)

Instead of periodic snapshots:

Redis logs every write.

Example:

```text
SET user:1 John
INCR counter
SET rate:user123 45
```

These commands are appended to a file.

```text
appendonly.aof
```

After restart:

Redis replays the commands.

Disadvantages
Bigger File

```text
appendonly.aof
```

keeps growing.

Example:

```text
SET x 1
SET x 2
SET x 3
SET x 4
```

All commands are stored.

Slower Restart

Redis must replay commands.

```text
1 million commands
```

takes longer than loading one snapshot.

### Method 3: Both RDB + AOF Together (Common Production Setup)

This is what many production systems use.

```text
          Redis
             |
     +-------+-------+
     |               |
    RDB             AOF
```

## Why Both?

RDB provides:

Fast backup
Fast recovery

AOF provides:

Better durability

Best of both worlds.

## Recovery Flow

Suppose Redis restarts.

Redis checks:

```text
appendonly.aof
```

If present:

Use AOF

because it contains newer data.

If AOF unavailable:

Use RDB

## Imagine AOF without Rewrite

You have one key:

```text
counter = 0
```

Now your application does:

```text
INCR counter
INCR counter
INCR counter
INCR counter
INCR counter
```

AOF stores every command:

```text
INCR counter
INCR counter
INCR counter
INCR counter
INCR counter
```

Current value in Redis:

```text
counter = 5
```

After 1 Million Increments

AOF becomes:

```text
INCR counter
INCR counter
INCR counter
...
1,000,000 times
```

But the actual data in Redis is just:

```text
counter = 1000000
```

Notice the problem:

Redis memory contains one value
AOF contains 1 million commands

The file becomes huge.

## What AOF Rewrite Does

Redis looks at the current state of memory:

```text
counter = 1000000
```

Instead of storing:

```text
INCR counter
INCR counter
INCR counter
...
1,000,000 commands
```

it creates a new compact AOF:

```text
SET counter 1000000
```

Both produce the same final result after recovery.

AOF is not storing data. AOF is storing commands.

## Step 1: Redis is Empty

Redis Memory:
(empty)

You run:

```text
SET name John
```

Now memory becomes:

```text
name = John
```

AOF stores:

```text
SET name John
```

## Step 2: Update the Value

You run:

```text
SET name Alice
```

Memory now:

```text
name = Alice
```

AOF now contains:

```text
SET name John
SET name Alice
```

Notice Redis didn't delete the old command from AOF.

AOF is keeping a history of writes.

## Step 3: More Updates

You run:

```text
SET name Bob
SET name Charlie
```

Memory becomes:

```text
name = Charlie
```

AOF becomes:

```text
SET name John
SET name Alice
SET name Bob
SET name Charlie
```

## Why Is This Wasteful?

Imagine Redis crashes.

To rebuild memory, Redis reads AOF:

```text
SET name John
```

Memory:

```text
name = John
```

Then:

```text
SET name Alice
```

Memory:

```text
name = Alice
```

Then:

```text
SET name Bob
```

Memory:

```text
name = Bob
```

Then:

```text
SET name Charlie
```

Memory:

```text
name = Charlie
```

Final result:

```text
name = Charlie
```

Now ask yourself:

Do we really need the old values?
John
Alice
Bob

No.

The only thing that matters is the final state:

```text
name = Charlie
```
