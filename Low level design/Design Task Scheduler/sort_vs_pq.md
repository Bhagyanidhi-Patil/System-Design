# sort vs priority_queue

`sort` and `priority_queue` interpret the comparator differently.

They use the comparator for different purposes.

| STL Function | Comparator Meaning | `a < b` | `a > b` |
| --- | --- | --- | --- |
| `sort()` | Should `a` come before `b`? | Ascending order | Descending order |
| `priority_queue` | Does `a` have lower priority than `b`? | Max Heap (`less<T>`) | Min Heap (`greater<T>`) |

## 1. sort()

For `sort`, the comparator answers:

> "Should a come before b?"

Example:

```cpp
sort(v.begin(), v.end(),
     [](int a, int b){
         return a < b;
     });
```

Meaning:

If `a < b`

↓

Place `a` before `b`

So:

```
5 2 10
```

↓

```
2 5 10
```

Ascending.

Similarly,

```cpp
return a > b;
```

means:

If `a > b`

↓

Place `a` before `b`

Result:

```
10 5 2
```

Descending.

## 2. priority_queue

For `priority_queue`, the comparator answers a different question:

> "Does `a` have LOWER priority than `b`?"

This is the important difference.

Suppose:

```cpp
priority_queue<
    int,
    vector<int>,
    greater<int>
> pq;
```

Now compare:

- `a = 5`
- `b = 2`

Comparator:

```cpp
greater<int>()
```

↓

`5 > 2`

↓

`true`

This means:

`5` has LOWER priority than `2`

Therefore:

**Top**

`2`

So the smallest element comes to the top.

## Visual Comparison

### sort

```cpp
return a < b;
```

Means:

`a comes BEFORE b`

### priority_queue

```cpp
return a < b;
```

Means:

`a has LOWER priority than b`

Hence `b` moves toward the top.

## Why it behaves differently

### Sort

`a < b`

↓

Ascending

### Priority Queue

`less<int>`

↓

Largest has highest priority

↓

Max Heap

`greater<int>`

↓

Smallest has highest priority

↓

Min Heap

## Easy way to remember

- `sort`
  - Comparator decides: Who comes first?
- `priority_queue`
  - Comparator decides: Who has lower priority?

