#include <iostream>
#include <list>
#include <vector>
using namespace std;

class Map {
private:
    struct Node {
        int key, val, gen;
    };

    int cap;
    int generation = 1;
    vector<list<Node>> table;

    int hash(int key) {
        return ((key % cap) + cap) % cap;
    }

public:
    Map(int c = 10) {
        cap = c;
        table.resize(cap);
    }

    void insert(int key, int val) {
        int index = hash(key);

        for (auto &it : table[index]) {
            if (it.gen == generation && it.key == key) {
                it.val = val;
                return;
            }
        }

        table[index].push_back({key, val, generation});
    }

    int get(int key) {
        int index = hash(key);

        for (auto &it : table[index]) {
            if (it.gen == generation && it.key == key)
                return it.val;
        }

        return -1;
    }

    void remove(int key) {
        int index = hash(key);

        for (auto it = table[index].begin();
             it != table[index].end(); ++it) {
            if (it->gen == generation && it->key == key) {
                table[index].erase(it);
                return;
            }
        }
    }

    void clear() {
        generation++;
    }

    void printtable() {
        for (int i = 0; i < cap; i++) {
            for (auto &it : table[i]) {
                if (it.gen == generation)
                    cout << it.key << ":" << it.val << ", ";
            }
        }
        cout << endl;
    }
};

int main() {
    Map mp;

    mp.insert(1, 10);
    mp.insert(2, 20);
    mp.insert(11, 30);

    mp.printtable();

    mp.clear();

    mp.printtable();           // Empty
    cout << mp.get(1) << endl; // -1

    mp.insert(1, 100);
    mp.printtable();           // 1:100
}