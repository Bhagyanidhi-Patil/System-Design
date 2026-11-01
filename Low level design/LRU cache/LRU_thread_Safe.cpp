/*
For your LRU Cache, std::lock_guard<std::mutex> is the better choice for the basic implementation. 
However, std::shared_mutex can provide better concurrency in certain situations.

With shared_mutex, we use:
- std::shared_lock for reading.
- std::unique_lock or std::lock_guard for writing.

For the LRU implementation:

get(key):
1. Find the key in the hashmap.
2. Remove the node from its current position.
3. Move the node to the front.
4. Return the value.
Although get() looks like a read operation, it modifies the doubly linked list because it updates the LRU ordering.

Therefore, it requires an exclusive lock.

put(key, value):
1. Check whether the key exists.
2. Update the value or insert a new node.
3. Evict the LRU node if necessary.
4. Update the hashmap and linked list.
It also requires an exclusive lock.

std::mutex + std::lock_guard is the better choice for your current LRU Cache.
*/


#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <mutex>
using namespace std;

struct Node {
public:
    int key, value;
    Node* prev;
    Node* next;

    Node(int k, int v)
        : key(k), value(v), prev(nullptr), next(nullptr) {}
};

class LRUCache {
private:
    size_t capacity;
    unordered_map<int, Node*> cache;

    Node* head;
    Node* tail;

    mutex mtx;  // Mutex to protect shared resources

    void remove(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void insertToFront(Node* node) {
        node->next = head->next;
        node->prev = head;

        node->next->prev = node;
        head->next = node;
    }

public:
    LRUCache(int cap) : capacity(cap) {
        if (capacity == 0)
            throw invalid_argument("Capacity must be > 0");

        head = new Node(0, 0);
        tail = new Node(0, 0);

        head->next = tail;
        tail->prev = head;
    }

    // Disable copy
    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;

    ~LRUCache() {
        // Cache must not be accessed by other threads during destruction
        Node* curr = head;

        while (curr) {
            Node* next = curr->next;
            delete curr;
            curr = next;
        }
    }

    int get(int key) {
        lock_guard<mutex> lock(mtx);

        auto it = cache.find(key);

        if (it == cache.end())
            return -1;

        Node* node = it->second;

        remove(node);
        insertToFront(node);

        return node->value;
    }

    void put(int key, int value) {
        lock_guard<mutex> lock(mtx);

        auto it = cache.find(key);

        // Key already exists
        if (it != cache.end()) {
            Node* node = it->second;

            node->value = value;

            remove(node);
            insertToFront(node);

            return;
        }

        // Evict LRU node
        if (cache.size() == capacity) {
            Node* lru = tail->prev;

            remove(lru);

            cache.erase(lru->key);

            delete lru;
        }

        // Insert new node
        Node* node = new Node(key, value);

        insertToFront(node);

        cache[key] = node;
    }
};

int main() {
    LRUCache cache(2);

    cache.put(1, 1);
    cache.put(2, 2);

    cout << cache.get(1) << endl;

    cache.put(3, 3);

    cout << cache.get(2) << endl;

    cache.put(4, 4);

    cout << cache.get(1) << endl;
    cout << cache.get(3) << endl;
    cout << cache.get(4) << endl;

    return 0;
}
