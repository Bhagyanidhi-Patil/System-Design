# Why do we need Compare?

Your priority_queue stores objects of type ScheduledTask.

```cpp
priority_queue<ScheduledTask,
               vector<ScheduledTask>,
               Compare> pq;
```

The C++ STL doesn't know how to compare two ScheduledTask objects.

For example, suppose the queue contains:

- Task A -> nextRun = 5 sec
- Task B -> nextRun = 2 sec
- Task C -> nextRun = 8 sec

How should it decide which task should be at the top?

- Compare task names?
- Compare addresses?
- Compare execution time?

It has no idea.

That's why we provide a comparator.

## What does the comparator do?

```cpp
struct Compare {
    bool operator()(ScheduledTask &a,
                    ScheduledTask &b)
    {
        return a.schedule.nextRun >
               b.schedule.nextRun;
    }
};
```

This tells the priority queue:

Compare two tasks based on their nextRun time.

## Why compare nextRun?

A scheduler should execute the task whose execution time is the earliest.

Example:

- Task A -> 5 sec
- Task B -> 2 sec
- Task C -> 8 sec

We want

**Top**

Task B (2 sec)

because it should execute first.

## Why > instead of <?

Remember:

By default,

```cpp
priority_queue<int>
```

is a max-heap.

Largest element comes first.

Example

```
5
2
10
```

Top

10

For a scheduler, we want Smallest nextRun at the top.

Using

```cpp
return a.nextRun > b.nextRun;
```

makes the priority queue behave like a min-heap.

## Default priority_queue

```cpp
priority_queue<int> pq;
```

This is actually equivalent to:

```cpp
priority_queue<
    int,
    vector<int>,
    less<int>
> pq;
```

Notice the comparator:

`less<int>`

`less<int>` means:

```text
a < b
```

So the largest element gets the highest priority.

## Using greater<>

```cpp
priority_queue<
    int,
    vector<int>,
    greater<int>
> pq;
```

Now the comparator becomes

`greater<int>`

which means

```text
a > b
```

Now the smallest element gets the highest priority.

| Priority Queue | Comparator | Heap Type |
| --- | --- | --- |
| `priority_queue<int>` | `less<int>` (default) | Max-Heap |
| `priority_queue<int, vector<int>, greater<int>>` | `greater<int>` | Min-Heap |
| `priority_queue<ScheduledTask, vector<ScheduledTask>, Compare>` | Your custom Compare | Ordered by nextRun (acts as a Min-Heap) |

