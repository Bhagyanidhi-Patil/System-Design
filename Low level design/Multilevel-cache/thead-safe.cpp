
#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <mutex>
#include <vector>
using namespace std;

// Common interface for all storage tiers
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

class LRUCache : public StorageTier {
private:
    size_t capacity;
    unordered_map<int, Node*> cache;
    Node* head;
    Node* tail;

    void remove(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void insertToFront(Node* node) {
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }

public:
    explicit LRUCache(int cap) : capacity(cap) {
        if (cap <= 0)
            throw invalid_argument("Capacity must be > 0");

        head = new Node(0, 0);
        tail = new Node(0, 0);

        head->next = tail;
        tail->prev = head;
    }

    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;

    ~LRUCache() override {
        Node* curr = head;

        while (curr) {
            Node* next = curr->next;
            delete curr;
            curr = next;
        }
    }

    // Called through MultiTierCache's mutex
    int get(int key) override {
        auto it = cache.find(key);

        if (it == cache.end())
            return -1;

        Node* node = it->second;
        remove(node);
        insertToFront(node);

        return node->value;
    }

    void put(int key, int value) override {
        auto it = cache.find(key);

        if (it != cache.end()) {
            Node* node = it->second;
            node->value = value;
            remove(node);
            insertToFront(node);
            return;
        }

        if (cache.size() == capacity) {
            Node* lru = tail->prev;
            remove(lru);
            cache.erase(lru->key);
            delete lru;
        }

        Node* node = new Node(key, value);
        insertToFront(node);
        cache[key] = node;
    }

    void erase(int key) override {
        auto it = cache.find(key);

        if (it == cache.end())
            return;

        Node* node = it->second;
        remove(node);
        cache.erase(it);
        delete node;
    }
};

class MultiTierCache {
private:
    vector<StorageTier*> tiers;  // Fastest to slowest
    mutable mutex mtx;

public:
    // Registers a tier. The cache does not own the tier object.
    void addTier(StorageTier* tier) {
        if (tier == nullptr)
            throw invalid_argument("Tier cannot be null");

        lock_guard<mutex> lock(mtx);
        tiers.push_back(tier);
    }

    int get(int key) {
        lock_guard<mutex> lock(mtx);

        for (size_t i = 0; i < tiers.size(); i++) {
            int value = tiers[i]->get(key);

            if (value != -1) {
                // Promote the value to all faster tiers
                for (size_t j = 0; j < i; j++)
                    tiers[j]->put(key, value);

                return value;
            }
        }

        return -1;
    }

    void put(int key, int value) {
        lock_guard<mutex> lock(mtx);

        // Write-through: write to every tier
        for (StorageTier* tier : tiers)
            tier->put(key, value);
    }

    void erase(int key) {
        lock_guard<mutex> lock(mtx);

        // Delete from every tier
        for (StorageTier* tier : tiers)
            tier->erase(key);
    }
};

int main() {
    LRUCache ram(2);
    LRUCache ssd(3);
    LRUCache hdd(5);

    MultiTierCache cache;

    cache.addTier(&ram);
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
