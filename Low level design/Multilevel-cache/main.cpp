
#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <vector>
using namespace std;

// Common interface for every tier
class StorageTier {
public:
    virtual int get(int key) = 0;
    virtual void put(int key, int value) = 0;
    virtual void erase(int key) = 0;
    virtual ~StorageTier() = default;
};

struct Node {
    int key, value;
    Node* prev;
    Node* next;

    Node(int k, int v)
        : key(k), value(v), prev(nullptr), next(nullptr) {}
};

// LRU Cache
class LRUCache : public StorageTier {
private:
    size_t capacity;
    unordered_map<int, Node*> m;
    Node* head;
    Node* tail;

    void remove(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void insertFront(Node* node) {
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
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

    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;

    ~LRUCache() {
        Node* curr = head;
        while (curr) {
            Node* next = curr->next;
            delete curr;
            curr = next;
        }
    }

    int get(int key) override {
        auto it = m.find(key);

        if (it == m.end())
            return -1;

        Node* node = it->second;
        remove(node);
        insertFront(node);

        return node->value;
    }

    void put(int key, int value) override {
        auto it = m.find(key);

        if (it != m.end()) {
            Node* node = it->second;
            node->value = value;
            remove(node);
            insertFront(node);
            return;
        }

        if (m.size() == capacity) {
            Node* lru = tail->prev;
            remove(lru);
            m.erase(lru->key);
            delete lru;
        }

        Node* node = new Node(key, value);
        insertFront(node);
        m[key] = node;
    }

    void erase(int key) override {
        auto it = m.find(key);

        if (it == m.end())
            return;

        Node* node = it->second;
        remove(node);
        m.erase(it);
        delete node;
    }
};

// Different tiers reuse the LRU implementation
class RamTier : public LRUCache {
public:
    RamTier(int capacity) : LRUCache(capacity) {}
};

class SsdTier : public LRUCache {
public:
    SsdTier(int capacity) : LRUCache(capacity) {}
};

class HddTier : public LRUCache {
public:
    HddTier(int capacity) : LRUCache(capacity) {}
};

// New tier can be added without changing MultiTierCache
class NvmeTier : public LRUCache {
public:
    NvmeTier(int capacity) : LRUCache(capacity) {}
};

// Multi-tier manager
class MultiTierCache {
private:
    vector<StorageTier*> tiers;  // Fastest to slowest

public:
    void addTier(StorageTier* tier) {
        tiers.push_back(tier);
    }

    int get(int key) {
        for (int i = 0; i < tiers.size(); i++) {
            int value = tiers[i]->get(key);

            if (value != -1) {
                // Promote to all faster tiers
                for (int j = 0; j < i; j++)
                    tiers[j]->put(key, value);

                return value;
            }
        }

        return -1;
    }

    void put(int key, int value) {
        for (StorageTier* tier : tiers)
            tier->put(key, value);
    }

    void erase(int key) {
        for (StorageTier* tier : tiers)
            tier->erase(key);
    }
};

int main() {
    RamTier ram(2);
    NvmeTier nvme(3);
    SsdTier ssd(5);
    HddTier hdd(10);

    MultiTierCache cache;

    // Register tiers in priority order
    cache.addTier(&ram);
    cache.addTier(&nvme);
    cache.addTier(&ssd);
    cache.addTier(&hdd);

    cache.put(1, 100);
    cache.put(2, 200);
    cache.put(3, 300);

    cout << cache.get(1) << endl;  // 100
    cout << cache.get(2) << endl;  // 200
    cout << cache.get(4) << endl;  // -1

    cache.put(1, 500);
    cout << cache.get(1) << endl;  // 500

    cache.erase(2);
    cout << cache.get(2) << endl;  // -1

    return 0;
}
