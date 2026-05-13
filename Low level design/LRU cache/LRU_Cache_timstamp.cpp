/*
Design LRU cache with following operations :
put(key,value, ttL) and get(key)
which returns value of the given key and if time is expired then it returns null.
If capacity of cache is full then remove least recently used value from the cache. 
Here each value which goes in cache has time to live, and it gets expired after ttl is expired.
key and value are in string to keep it simple.
*/

#include <iostream>
#include <unordered_map>
#include <thread>
#include <chrono>
using namespace std;

struct Node{
    string key;
    string value;
    chrono::steady_clock::time_point expiryTime;
    Node* next;
    Node* prev;

    Node(string k,string v,chrono::steady_clock::time_point exp)
    :key(k),value(v),expiryTime(exp),prev(NULL),next(NULL){}
};

class LRUCache{
private:
    unordered_map<string,Node*>cache;
    int capacity;
    Node* head;
    Node* tail;

    void removeNode(Node* node){
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void insertFront(Node* node){
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }

    void moveToFront(Node* node){
        removeNode(node);
        insertFront(node);
    }

    void deleteNode(Node* node){
        removeNode(node);
        cache.erase(node->key);
        delete node;
    }

    bool isExpired(Node* node){
        return chrono::steady_clock::now()>=node->expiryTime;
    }

public:
    LRUCache(int cap):capacity(cap){
        head = new Node("","",chrono::steady_clock::now());
        tail = new Node("","",chrono::steady_clock::now());
        head->next = tail;
        tail->prev = head;
    }

    string get(string key){
        if(cache.find(key)==cache.end()){
            return "null";
        }
        Node* node = cache[key];
        if(isExpired(node)){
            deleteNode(node);
            return "null";
        }
        moveToFront(node);
        return node->value;
    }

    void put(string key,string value,int ttlseconds){
        auto expiry = chrono::steady_clock::now()+chrono::seconds(ttlseconds);

        if(cache.find(key)!=cache.end()){
            Node* node = cache[key];
            node->value = value;
            node->expiryTime = expiry;
            moveToFront(node);
            return;
        }
        if(cache.size()>=capacity){
            Node* lru_node = tail->prev;
            deleteNode(lru_node);
        }
        Node* newnode = new Node(key,value,expiry);
        cache[key] = newnode;
        insertFront(newnode);
    }
};

int main() {

    LRUCache cache(2);

    cache.put("a", "apple", 5);

    cout << cache.get("a") << endl;

    this_thread::sleep_for(chrono::seconds(6));

    cout << cache.get("a") << endl;

    return 0;
}

/*
------------------------------------------------------------
LRU CACHE WITH TTL (TIME TO LIVE) DESIGN
------------------------------------------------------------

GOAL:
Design a cache that supports:

1. put(key, value, ttl)
2. get(key)

AND:
- If key is expired → return "null"
- If capacity is full → evict LRU (Least Recently Used)
- Each key has its own TTL (Time To Live)

------------------------------------------------------------
CORE IDEA
------------------------------------------------------------

We combine 3 things:

1. HashMap (unordered_map)
   -> key -> Node*
   -> O(1) access

2. Doubly Linked List (DLL)
   -> maintains LRU order
   -> head = most recently used
   -> tail = least recently used

3. TTL (Expiration Time)
   -> expiryTime = now + ttl
   -> stored inside each node

------------------------------------------------------------
NODE STRUCTURE
------------------------------------------------------------

Each node contains:

- key
- value
- expiryTime (absolute timestamp)
- prev pointer
- next pointer

------------------------------------------------------------
HOW TTL WORKS
------------------------------------------------------------

During PUT:
----------------
expiryTime = currentTime + ttl

Example:
currentTime = 100
ttl = 5 sec
expiryTime = 105

So node is valid until time = 105

------------------------------------------------------------
During GET:
----------------
Check:
if (currentTime >= expiryTime)
    -> expired
    -> remove node
    -> return null
else
    -> valid
    -> move to front (LRU update)
    -> return value

------------------------------------------------------------
LRU POLICY
------------------------------------------------------------

When capacity is full:
- remove node from tail (least recently used)
- delete from hashmap
- delete from DLL

------------------------------------------------------------
WHY STEADY CLOCK?
------------------------------------------------------------

We use:
    chrono::steady_clock

Because:
- It is monotonic (always increases)
- Not affected by system time changes

Avoid system_clock because:
- system time can be changed manually
- TTL logic would break

------------------------------------------------------------
TIME COMPLEXITY
------------------------------------------------------------

get():
    O(1)

put():
    O(1)

eviction:
    O(1)

------------------------------------------------------------
WHY THIS DESIGN WORKS WELL
------------------------------------------------------------

- Fast lookups (hashmap)
- Fast eviction (DLL)
- Correct expiration (TTL check on access)
- Scalable for cache systems

------------------------------------------------------------
REAL-WORLD USE
------------------------------------------------------------

Similar design used in:
- caching systems
- session stores
- rate limiting systems

(e.g. Redis-style TTL behavior)

------------------------------------------------------------
WHY WE USE unordered_map IN LRU ?
------------------------------------------------------------
We need to quickly access a cache entry using its key.

WITHOUT unordered_map: If we only use a Doubly Linked List (DLL):

To find a key:
    we must traverse the list

Time Complexity:
    O(n)

Problems:
- get() becomes slow
- put() updates become slow
- TTL check requires traversal
- overall cache becomes inefficient

WITH unordered_map:
We store: key → pointer to node in DLL

So we can directly access the node.
Time Complexity:
    O(1) average
------------------------------------------------------------
*/