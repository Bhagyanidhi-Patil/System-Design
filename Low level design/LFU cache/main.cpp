#include <iostream>
#include <unordered_map>
using namespace std;

class Node{
public:
    int key,value,freq;
    Node* next;
    Node* prev;

    Node(int k,int v){
        key = k;
        value = v;
        freq = 1;
        prev = NULL;
        next = NULL;
    }
};

class DoublyLinkedList{
private:
    Node* head;
    Node* tail;
    int size;
public:
    DoublyLinkedList(){
        head = new Node(0,0);
        tail = new Node(0,0);
        head->next = tail;
        tail->prev = head;
        size = 0;
    }

    ~DoublyLinkedList(){
        delete head;
        delete tail;
    }

    bool empty(){
        return size == 0;
    }

    void insertFront(Node* node){
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
        size++;
    }

    void remove(Node* node){
        node->prev->next = node->next;
        node->next->prev = node->prev;
        size--;
    }

    Node* removeLast(){
        if(empty())return nullptr;
        Node* node = tail->prev;
        remove(node);
        return node;
    }
};

class LFUCache{
private:
    int capacity;
    int minFreq;
    unordered_map<int,Node*>keyMap;
    unordered_map<int,DoublyLinkedList*>freqMap;

    void updateFrequency(Node* node){
        int oldFreq = node->freq;
        DoublyLinkedList* oldList = freqMap[oldFreq];
        oldList->remove(node);
        if(oldList->empty()){
            freqMap.erase(oldFreq);
            delete oldList;

            if(minFreq ==oldFreq)minFreq++;
        }
        node->freq++;
        if(freqMap.find(node->freq)==freqMap.end()){
            freqMap[node->freq] = new DoublyLinkedList();
        }
        freqMap[node->freq]->insertFront(node);
    }
public:
    LFUCache(int cap){
        if(cap<0)
            throw invalid_argument("Capacity must be >0");
        capacity = cap;
        minFreq = 0;
    }
    LFUCache(const LFUCache&)=delete;
    ~LFUCache(){
        for(auto &it:keyMap)
            delete it.second;
        for(auto &it:freqMap)
            delete it.second;
    }

    int get(int key){
        if(keyMap.find(key)==keyMap.end())
            return -1;
        Node* node = keyMap[key];
        updateFrequency(node);
        return node->value;
    }

    void put(int key,int value){
        if(capacity == 0)return;

        if(keyMap.find(key)!=keyMap.end()){
            Node* node = keyMap[key];
            node->value = value;
            updateFrequency(node);
            return;
        }
        if(keyMap.size()==capacity){
            DoublyLinkedList* list = freqMap[minFreq];
            Node* vicitim = list->removeLast();
            keyMap.erase(vicitim->key);
            delete vicitim;
            if(list->empty()){
                freqMap.erase(minFreq);
                delete list;
            }
        }
        Node* newNode = new Node(key,value);
        minFreq = 1;
        if(freqMap.find(1)==freqMap.end())
            freqMap[1] = new DoublyLinkedList();
        freqMap[1]->insertFront(newNode);
        keyMap[key] = newNode;
    }
};

int main() {

    LFUCache cache(2);

    cache.put(1, 10);
    cache.put(2, 20);

    cout << cache.get(1) << endl;   // 10

    cache.put(3, 30);               // evicts key 2

    cout << cache.get(2) << endl;   // -1
    cout << cache.get(3) << endl;   // 30

    cache.put(4, 40);               // evicts key 1

    cout << cache.get(1) << endl;   // -1
    cout << cache.get(3) << endl;   // 30
    cout << cache.get(4) << endl;   // 40
}