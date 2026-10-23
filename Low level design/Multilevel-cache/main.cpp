#include <iostream>
#include <unordered_map>
#include <stdexcept>
using namespace std;

class StorageTier{
public:
    virtual int get(int key)=0;
    virtual void put(int key,int value)=0;
    virtual void erase(int key)=0;
    virtual ~StorageTier()=default;
};

struct Node{
    int key,value;
    Node* prev;
    Node* next;

    Node(int k,int v):key(k),value(v),next(nullptr),prev(nullptr){}
};

class LRUCache:public StorageTier{
private:
    size_t capacity;
    unordered_map<int,Node*>m;
    Node* head;
    Node* tail;
    
    void remove(Node* node){
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void insertFront(Node* node){
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }

public:
    LRUCache(int cap):capacity(cap){
        if(capacity==0)
            throw invalid_argument("Capacity must be >0");
        head = new Node(0,0);
        tail = new Node(0,0);

        head->next = tail;
        tail->prev = head;
    }

    ~LRUCache() {
        Node* curr = head;
        while (curr) {
            Node* next = curr->next;
            delete curr;
            curr = next;
        }
    }

    int get(int key)override{
        if(m.find(key)==m.end())
            return -1;
        Node* node = m[key];
        remove(node);
        insertFront(node);
        return node->value;
    }

    void put(int key,int value)override{
        if(m.find(key)!=m.end()){
            Node* node = m[key];
            node->value = value;
            remove(node);
            insertFront(node);
            return;
        }
        if(m.size()==capacity){
            Node* lrunode = tail->prev;
            remove(lrunode);
            m.erase(lrunode->key);
            delete lrunode;
        }
        Node* node = new Node(key,value);
        insertFront(node);
        m[key] = node;
    }

    void erase(int key)override{
        if(m.find(key)==m.end()){
            return;
        }
        Node* node = m[key];
        remove(node);
        m.erase(key);
        delete node;
    }
};

//mutli-tier cache manager
class MultiTierCache{
private:
    StorageTier* tiers[3];
    int tiercount = 0;
public:
    void addTier(StorageTier* tier){
        if(tiercount<3)
            tiers[tiercount++] = tier;
    }

    int get(int key){
        //search RAM -> SSD -> HDD
        for(int i=0;i<tiercount;i++){
            int value = tiers[i]->get(key);
            if(value!=-1){
                for(int j=0;j<i;j++){
                    tiers[j]->put(key,value);
                }
                return value;
            }
        }
        return -1;
    }

        void put(int key, int value) {
        // Write through to all tiers
        for (int i = 0; i < tiercount; i++)
            tiers[i]->put(key, value);
    }

    void erase(int key) {
        // Delete the key from every tier
        for (int i = 0; i < tiercount; i++)
            tiers[i]->erase(key);
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
    cout << cache.get(4) << endl;  // -1: cache miss

    cache.put(1, 500);
    cout << cache.get(1) << endl;  // 500

    cache.erase(2);
    cout << cache.get(2) << endl;  // -1

    return 0;
}