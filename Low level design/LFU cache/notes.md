# LFU Implementation Notes

In LFU implementation, we maintain two maps: `keyMap` and `FreqMap`.

## Key Map and Freq Map visual representation. 

```text
+-------------------+
| 1 ---> Node1      |
| 2 ---> Node2      |
| 3 ---> Node3      |
| 4 ---> Node4      |
+-------------------+
          |
          |
          v
+-------------------+
| Actual Nodes     |
+-------------------+
```

```text
Node1 (freq=3)
Node2 (freq=1)
Node3 (freq=2)
Node4 (freq=1)
```

```text
                  ^
                  |
                  |

             freqMap

1 ----------> DLL : 4 <-> 2
2 ----------> DLL : 3
3 ----------> DLL : 1
```

```text
minFreq = 1
```

## Why do we need keyMap?

`keyMap` is needed to find a node in $O(1)$.
Without it, `get(key)` would become $O(n)$.

## What is keyMap?

```cpp
unordered_map<int, Node*> keyMap;
```

It stores:

```text
key  --->  Node*
```

Example:

```text
1 ---> Node1
2 ---> Node2
3 ---> Node3
```

So if someone asks:

```cpp
get(3);
```

we immediately get:

```cpp
Node* node = keyMap[3];
```

in $O(1)$.

## What happens without keyMap?

Suppose the cache contains:

```text
Freq1

2 <-> 5
Freq2

3
Freq4

1
```

Now call:

```cpp
get(3);
```

How do you find node 3?

You would have to search:

```text
Freq1

2

5
```

Not found.

Search:

```text
Freq2

3
```

Found.

Worst case?

Search every list.

That becomes:

```text
O(n)
```

which breaks the LFU requirement.

## updateFrequency function explaination :

1. Remove the node from its current frequency list.
2. If that frequency list becomes empty, remove the list.
3. Increase the node's frequency.
4. Insert the node into the new frequency list.

## Initial State

Suppose the cache contains:

```text
Key    Value    Frequency
1      10       2
2      20       1
3      30       1
```

Our data structures are:

```text
keyMap

1 -> Node1
2 -> Node2
3 -> Node3

freqMap

1 ---> [3 <-> 2]
         MRU    LRU

2 ---> [1]

minFreq = 1
```

Now we execute:

```cpp
get(3);
```

So node 3 moves from frequency 1 to frequency 2.

### Line 1

```cpp
int oldFreq = node->freq;
```

Node 3 currently has:

```text
freq = 1
```

So:

```cpp
oldFreq = 1;
```

We save the old frequency because after incrementing `node->freq`, we will still need to access the old list.

### Line 2

```cpp
DoublyLinkedList* oldList = freqMap[oldFreq];
```

This means:

```cpp
oldList = freqMap[1];
```

So `oldList` points to:

```text
Freq 1

3 <-> 2
```

Nothing has changed yet.

### Line 3

```cpp
oldList->remove(node);
```

Remove node 3 from the frequency-1 list.

Before:

```text
Freq1

3 <-> 2
```

After:

```text
Freq1

2
```

Notice:

Node 3 is not deleted. It is only unlinked from this list. The object still exists because `keyMap` still points to it.

### Line 4

```cpp
if (oldList->empty())
```

We ask:

> After removing this node, did this frequency list become empty?

In our example:

Frequency 1 still contains node 2.

```text
Freq1

2
```

So the answer is No.

We skip this block.

Let's see another example.

Suppose instead:

```text
Freq1

3
```

Only one node.

After removing 3:

```text
Freq1

(empty)
```

Now this if becomes true.

### Line 5

```cpp
freqMap.erase(oldFreq);
```

Removes:

```text
1 ---> DLL
```

from the hash map.

Because there are no nodes with frequency 1 anymore.

### Line 6

```cpp
delete oldList;
```

Frees the memory occupied by that empty DLL.

### Line 7

```cpp
if (minFreq == oldFreq)
    minFreq++;
```

This line confuses many people.

Let's understand it.

Suppose the cache is:

```text
Freq1

3
Freq2

1
minFreq = 1
```

Now `get(3)` happens.

Node 3 leaves frequency 1.

Frequency 1 becomes empty.

The smallest frequency in the cache is now:

```text
2
```

So:

```cpp
minFreq++;
```

changes:

```text
1
2
```

Now `minFreq` is correct again.

### Line 8

```cpp
node->freq++;
```

Increase node frequency.

Before:

```text
Node3

freq = 1
```

After:

```text
Node3

freq = 2
```

Now this node belongs to the frequency-2 list.

### Line 9

```cpp
if (freqMap.find(node->freq) == freqMap.end())
```

We ask:

> Does a frequency-2 list already exist?

**Case 1:**

It already exists.

```text
Freq2

1
```

Nothing to do.

**Case 2:**

Suppose frequency 2 doesn't exist.

Then we create it.

```cpp
freqMap[node->freq] = new DoublyLinkedList();
```

Now:

```text
Freq2

(empty)
```

exists.

### Line 10

```cpp
freqMap[node->freq]->insertFront(node);
```

Insert the node into its new frequency list.

Suppose frequency 2 already had:

```text
Freq2

1
```

After insertion:

```text
Freq2

3 <-> 1
```

**Why at the front?**

Because node 3 was just accessed, making it the most recently used among nodes with frequency 2.

If we later need to evict from frequency 2, the tail will still be the least recently used node.
