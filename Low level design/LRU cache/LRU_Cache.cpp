/*
Design LRU Cache: Use,
    • unordered_map → key → node pointer (O(1) lookup)
    • doubly linked list → maintain usage order
    • most recently used (MRU) at front
    • least recently used (LRU) at back
💡 Follow-ups: • thread safety? distributed cache version?

• get(key) → lookup in map → move node to front → O(1)
• put(key, value) → insert/update → move to front → O(1)
• eviction → remove from tail → O(1)
*/

#include <iostream>
#include <unordered_map>
#include <stdexcept>
using namespace std;

struct Node{
public:
    int key,value;
    Node* prev;
    Node* next;

    Node(int k,int v):key(k),value(v),prev(nullptr),next(nullptr){}
};

class LRUCache{
private:
    size_t capacity;
    unordered_map<int,Node*>cache;
    Node* head;
    Node* tail;

    void remove(Node* node){
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void insertToFront(Node* node){    //insert next to head node, as head is dummy node
        node->next = head->next;
        node->prev = head;
        node->next->prev = node;
        head->next = node;
    }
public:
    //You don’t need to initialize unordered_map in the constructor because it is automatically default-constructed to an empty map when the object is created.
    LRUCache(int cap){
        capacity = cap;
        if(capacity==0)
            throw invalid_argument("Capacity must be >0");
        head = new Node(0,0);
        tail = new Node(0,0);
        head->next = tail;
        tail->prev = head;
    }
    //disable copy 
    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;

    ~LRUCache(){
        Node* curr = head;
        while(curr){
            Node* next = curr->next;
            delete curr;
            curr = next;
        }
    }

    int get(int key){
        if (cache.find(key) == cache.end()) return -1;
        auto it = cache.find(key);
        Node* node = it->second;
        remove(node);
        insertToFront(node);
        return node->value;
    }

    void put(int key,int value){
        if(cache.find(key)!=cache.end()){
            Node* node = cache.find(key)->second;
            node->value = value;
            remove(node);
            insertToFront(node);
            return;
        }
        if(cache.size()==capacity){
            Node* lru = tail->prev;
            remove(lru);
            cache.erase(lru->value);
            delete lru;
        }
        Node* node = new Node(key,value);
        insertToFront(node);
        cache[key] = node;
    }
};


int main() {
    LRUCache cache(2);  // capacity = 2

    cache.put(1, 1);
    cache.put(2, 2);

    cout << cache.get(1) << endl; // returns 1

    cache.put(3, 3);  // evicts key 2

    cout << cache.get(2) << endl; // returns -1 (not found)

    cache.put(4, 4);  // evicts key 1

    cout << cache.get(1) << endl; // returns -1 (not found)
    cout << cache.get(3) << endl; // returns 3
    cout << cache.get(4) << endl; // returns 4

    return 0;
}

/*
Thread Safety:
Your current LRU is not thread-safe:
    unordered_map + DLL updates are not atomic
    Concurrent get/put → race conditions, corruption

 “For thread safety, my current LRU cache isn’t safe because both the unordered_map and doubly linked list are modified concurrently,
which can lead to race conditions.The simplest solution is to use a single mutex and wrap both get and put with a lock. 
That guarantees correctness but limits concurrency since only one thread can access the cache at a time.   
std::mutex mtx;

int get(int key) {
    std::lock_guard<std::mutex> lock(mtx);
    ...
}

void put(int key, int value) {
    std::lock_guard<std::mutex> lock(mtx);
    ...
}

Distributed Cache Version:
Basic Architecture
1. Partitioning :- Use: consistent hashing .Each server stores part of the keyspace.
2. Each node runs its own LRU ,handles eviction locally
3. Request flow
Client: key → hash → node → get/put

⚖️ Key Challenges
1. Consistency
    Strong consistency → slow, complex
    Eventual consistency → fast, common
2. Replication
    Store copies across nodes
    Improves availability
    Complicates eviction
3. Eviction problem
    Each node evicts locally
    Not globally optimal

“For thread safety, my current LRU cache isn’t safe because both the unordered_map and doubly linked list are modified concurrently, 
which can lead to race conditions. The simplest solution is to use a single mutex and wrap both get and put with a lock. 
That guarantees correctness but limits concurrency since only one thread can access the cache at a time.
To improve scalability, I’d move to a sharded design. I’d split the cache into multiple independent LRU instances and
use hashing to map each key to a shard. Each shard would have its own lock, which significantly reduces contention and
improves parallelism. This is a common approach in high-performance systems.
For a distributed version, I’d partition data across multiple nodes using consistent hashing so that each node is 
responsible for a subset of keys. Each node would run its own local LRU cache and handle eviction independently.
There are trade-offs here. Eviction becomes approximate globally since each node only sees part of the data, 
and consistency depends on the system design. In practice, most systems use eventual consistency for better performance.
Real-world systems like Redis and Memcached follow similar principles, combining partitioning, replication,
and efficient eviction policies.
So overall, I’d start simple with locking for correctness, then scale using sharding and distributed partitioning depending on
system requirements.”

*/