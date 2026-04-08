/*
Question : Design Unordered map 

🧠 Core Idea
An unordered_map is built using:

✅ Hashing + Buckets + Collision handling

🧩 Components
    Hash function → maps key → index
    Buckets (array/vector) → stores elements
    Collision handling → multiple keys same index
*/

#include<iostream>
#include<vector>
#include <list>
using namespace std;

class UnorderedMap{
private:
    vector<list<pair<int,int>>>table;
    int capacity;

    int hash(int key){
        return key%capacity;
    }
public:
    UnorderedMap(int n=10){
        capacity = n;
        table.resize(n);
    }

    //function to insert or update value
    void insert(int key,int value){
        int index = hash(key);
        for(auto &it : table[index]){
            if(it.first == key){
                it.second = value;  //if key alreay present in map, update value
                return;
            }
        }
        table[index].push_back({key,value});  //if key not present then update key,value pair in table
    }

    //function to get the value using key
    int get(int key){
        int index = hash(key);
        for(auto &it:table[index]){
            if(it.first == key)
                return it.second;
        }
        return -1;
    }

    //function to remove the key
    void remove(int key){
        int index = hash(key);
        auto &bucket = table[index];

        for(auto it=bucket.begin();it!=bucket.end();++it){
            if(it->first == key){
                bucket.erase(it);  //remove the key-value pair from bucket
                return;
            }
        }
    }

    //function to print all key-value pairs in the map
    void printTable(){
        for(int i=0;i<capacity;i++){
            cout << "Bucket " << i << ": "<<endl;
            for(auto &it:table[i]){
                cout<<"Key: "<<it.first<<" Value: "<<it.second<<endl;
            }
            cout<<endl;
        }
    }
};

int main() {
    UnorderedMap mp;

    mp.insert(1, 10);
    mp.insert(2, 20);
    mp.insert(11, 30); // collision (same bucket as 1 if capacity=10)
    mp.printTable();
    cout << mp.get(1) << endl;  // 10
    cout << mp.get(11) << endl; // 30

    mp.remove(1);
    cout << mp.get(1) << endl;  // -1
    mp.printTable();
}

/*
NOTE:
1. We write,
    "auto& bucket = table[index];" 
    because, we want to modify the actual bucket in the table, not a copy of it.
    Here bucket is a reference. It points to the actual bucket inside table.

    If we write like this:
    "auto bucket = table[index];"
    Then bucket will be a copy of the list at table[index], and any modifications to bucket will not affect the original list in the table.
    👉 You are modifying the copy, NOT the original table.

-----------

2. for (auto &it : table[i]) {
    ....code....
    }
    Here,👉 No copy — directly refers to original element ,it is a reference.

    If we write like this:
    for (auto it : table[i]) {
    ....code....
    }
    Here,👉 it is a copy of the element in the list, and any modifications to it will not affect the original element in the list.
*/

/*
🧠 3 Cases (Very Important)
✅ 1. Read-only → use const auto& (BEST PRACTICE)
for (const auto& it : table[i]) {
    cout << it.first << " " << it.second << endl;
}

✅ 2. Modifying elements → use auto& (BEST PRACTICE)
for (auto& it : table[i]) {
    it.second += 10; // modify value
}   

✅ 3. Just need a copy → use auto
for (auto it : table[i]) {
    cout << it.first << " " << it.second << endl;
}
*/