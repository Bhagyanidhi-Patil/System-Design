# Task Scheduler — Low Level Design

Think of: Windows Task Scheduler, Linux Cron, Reminder apps, or any Job Scheduler.

## Examples

- Task 1: Run every 5s
- Task 2: Run after 10s
- Task 3: Run every day at 9:00 AM

The scheduler should execute these tasks automatically according to their schedules.

## Requirements

### Functional

- Create task
- Execute task
- Run after delay
- Run repeatedly (interval)
- Stop task
- Cancel task

### Non-functional

- Extensible (easy to add new schedule types)
- Thread-safe
- Efficient (scalable)

## Identify Objects

Ask: "What nouns do I see?"

- `Task`
- `Scheduler`
- `Schedule`
- `Worker Thread`

These become our main classes.

## Design — Classes

Task is an abstraction that knows how to execute itself:

```text
             +----------------+
             |     Task       |
             +----------------+
             | id             |
             | name           |
             | execute()      |
             +----------------+

                     ▲
                     |
             +----------------+
             |  PrintTask     |
             +----------------+
```

Instead of hardcoding timing logic inside `Scheduler`, create a separate `Schedule` class that knows when a task should run:

```text
              +--------------------+
              |     Schedule       |
              +--------------------+
              | nextExecutionTime  |
              | interval           |
              +--------------------+
```

Combine `Task` and `Schedule` into a `ScheduledTask`:

```text
+-----------------------+
| ScheduledTask         |
+-----------------------+
| Task*                 |
| Schedule              |
+-----------------------+
```

`Scheduler` manages `ScheduledTask` objects:

```text
+----------------------+
| Scheduler            |
+----------------------+
| priority queue       |
| worker thread        |
+----------------------+
| addTask()            |
| start()              |
| stop()               |
+----------------------+
```

Final class flow:

```text
             Task
               ▲
               │
         PrintTask

               │
               │
        ScheduledTask
        ----------------
        Task
        Schedule

               │
               ▼

          Scheduler
```

## Why a Priority Queue?

We need the earliest next-execution time to be processed first.

Example:

- Task A → 5s
- Task B → 20s
- Task C → 10s

Priority queue order: A, C, B (earliest `nextExecutionTime` at the top).

Time complexity:

- Insert: O(log n)
- Remove: O(log n)
- Peek: O(1)

This structure is well suited for scheduling.

---