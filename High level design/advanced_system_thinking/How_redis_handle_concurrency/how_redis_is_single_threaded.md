**When people say Redis is single-threaded, they usually mean that Redis processes most commands on a single main execution thread, one command at a time.**

### What it means

* Redis uses **one main thread** to execute commands.

* Commands are handled sequentially:

  ```
  Client 1 -> SET key value
  Client 2 -> GET key
  Client 3 -> INCR counter
  ```

  Redis executes them one after another, not simultaneously.

### Why is it still fast?

Redis keeps data **in memory (RAM)** and uses highly optimized data structures, so most operations take microseconds. By avoiding locks and thread synchronization overhead, Redis can often outperform multi-threaded systems for many workloads.

### Common misconception

Single-threaded **does not mean Redis uses only one CPU core for everything**.

Modern Redis versions use additional threads for some tasks, such as:

* Network I/O (reading/writing client connections)
* Persistence operations
* Replication-related work
* Background memory cleanup

But the actual execution of most Redis commands still happens on the main thread.

---

### 1. If Redis stores data in RAM, is data lost when the system restarts?

**It depends on configuration.**

By default, Redis can save data to disk using persistence mechanisms:

#### RDB (Snapshot)

Redis keeps the actual data in RAM because RAM is very fast.

For durability, Redis can periodically take a snapshot of the data in memory and save it to disk (RDB persistence).

Example:

Time 10:00

```text
user:1 = John
counter = 100
```

Redis creates a snapshot:

```text
dump.rdb
```

stored on disk.

Now suppose more changes happen:

```text
counter = 101
counter = 102
counter = 103
```

If the server crashes before the next snapshot, Redis restores from the last saved snapshot:

```text
counter = 100
```

The changes after the snapshot (101, 102, 103) are lost.

This is why RDB snapshots are fast and efficient, but some recent data may be lost between snapshots.

To reduce data loss, Redis also supports AOF (Append Only File), where every write operation is logged to disk. When Redis restarts, it replays those commands to rebuild the latest state.


#### AOF (Append Only File)

Redis logs every write command.

```text
SET name John
INCR counter
LPUSH users Alice
```

On restart, Redis replays these commands to rebuild memory.

**Benefit:** Much less data loss.

So Redis stores **working data in RAM** for speed, but can **persist data to disk** for recovery.

---

### 2. Does one CPU core always run one thread?

A CPU core can execute **one thread at a time** (ignoring technologies like Hyper-Threading for now).

Example:

```text
4 CPU cores
```

can execute:

```text
Core 1 → Thread A
Core 2 → Thread B
Core 3 → Thread C
Core 4 → Thread D
```

simultaneously.

So a 4-core CPU can truly run 4 threads in parallel.

---

### 3. Then why does Redis use only one thread for commands?

Imagine Redis has:

```text
counter = 5
```

Two threads try to do:

```text
INCR counter
```

at the same time.

Without synchronization:

```text
Thread A reads 5
Thread B reads 5
Thread A writes 6
Thread B writes 6
```

Final value:

```text
6
```

instead of:

```text
7
```

To avoid this, multi-threaded systems use locks, mutexes, semaphores, etc.

Redis avoids all that complexity by saying:

```text
Only one thread can modify data.
```

So:

```text
INCR counter
INCR counter
```

becomes:

```text
5 → 6 → 7
```

with no locks.

