 # LRU vs LFU

 | Feature | LRU Cache | LFU Cache |
 | --- | --- | --- |
 | Full Form | Least Recently Used | Least Frequently Used |
 | Eviction Criteria | Remove the item that was accessed least recently | Remove the item that was accessed the fewest times |
 | Tracks | Last access time/order | Access count (frequency) |
 | Tie Breaker | Not applicable | If frequencies are equal, remove the least recently used among them |
 | Data Structures | HashMap + Doubly Linked List | HashMap + Frequency Map (Frequency → Doubly Linked List) |

 ## Example

 **Capacity = 3**

 ```text
 put(1)
 put(2)
 put(3)
 ```

 Cache:

 `1  2  3`

 Now perform:

 ```text
 get(1)
 get(1)
 get(2)
 ```

 Access summary:

 | Key | Last Used | Frequency |
 | --- | --- | --- |
 | 1 | Most recent | 3 |
 | 2 | Recent | 2 |
 | 3 | Oldest | 1 |

 Now:

 `put(4)`

 ### LRU

 LRU only cares about recent usage.

 Last access order:

 `Oldest -----> Newest`

 `3   2   1`

 So it removes:

 `3`

 ### LFU

 LFU only cares about frequency.

 `1 -> freq = 3`
 `2 -> freq = 2`
 `3 -> freq = 1`

 Smallest frequency is:

 `3`

 So LFU also removes:

 `3`

 Here, both behave the same.

 ## A Case Where They Differ

 **Capacity = 2**

 ```text
 put(1)
 put(2)
 ```

 Now:

 ```text
 get(1)
 get(1)
 ```

 Frequencies:

 `1 -> 3`
 `2 -> 1`

 Now wait for some time and access:

 `get(2)`

 Current status:

 | Key | Frequency | Most Recent? |
 | --- | --- | --- |
 | 1 | 3 | No |
 | 2 | 2 | Yes |

 Now:

 `put(3)`

 ### LRU Decision

 Recent order:

 `Oldest → Newest`

 `1    2`

 So LRU removes:

 `1`

 because it hasn't been used recently.

 ### LFU Decision

 Frequency:

 `1 -> 3`
 `2 -> 2`

 LFU removes:

 `2`

 because it has been accessed fewer times overall.

 So the results differ:

 **LRU:**
 Remove 1

 **LFU:**
 Remove 2