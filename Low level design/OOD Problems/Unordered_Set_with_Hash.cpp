/* Question - Implement an unordered set using a hash table with separate chaining for collision resolution.
A set is a data structure that stores unique elements (no duplicates).

A 'set' is a data structure that stores unique elements in sorted (ordered) form, 
typically implemented using a self-balancing binary search tree.

An 'unordered_set' is a data structure that stores unique elements without any specific order,
typically implemented using a hash table to provide fast average-time operations.
*/

#include <iostream>
#include <list>
#include <vector>
using namespace std;

class UnorderedSet {
private:
    static const int N = 1009;   // number of buckets (prime)
    vector<list<int>> table;
    int count;

    int hashFunction(int x) {
        return (x % N + N) % N; // handles negative values
    }

public:
    UnorderedSet() {
    table = vector<list<int>>(N);  // create vector of size N
    count = 0;
    } 

    // INSERT , TC O(n) in worst case (all elements in same bucket), O(1) on average
    bool insert(int x) {
        int idx = hashFunction(x);

        for (int val : table[idx]) {
            if (val == x) return false; // no duplicates
        }

        table[idx].push_back(x);
        count++;
        return true;
    }

    // REMOVE TC O(n) in worst case, O(1) on average
    bool remove(int x) {
        int idx = hashFunction(x);

        for (auto it = table[idx].begin(); it != table[idx].end(); ++it) {
            if (*it == x) {
                table[idx].erase(it);
                count--;
                return true;
            }
        }
        return false;
    }

    // LOOKUP , TC O(n) in worst case, O(1) on average
    bool contains(int x) {
        int idx = hashFunction(x);

        for (int val : table[idx]) {
            if (val == x) return true;
        }
        return false;
    }

    // SIZE , TC O(1)
    int size() {
        return count;
    }

    // CLEAR , TC O(n) because we need to clear each bucket
    void clear() {
        for (int i = 0; i < N; i++) {
            table[i].clear();
        }
        count = 0;
    }

    // ITERATE , TC O(n) where n is number of elements in the set
    void iterate() {
        for (int i = 0; i < N; i++) {
            for (int val : table[i]) {
                cout << val << " ";
            }
        }
        cout << endl;
    }
};

int main() {
    UnorderedSet s;

    s.insert(10);
    s.insert(20);
    s.insert(30);
    s.insert(10); // duplicate ignored

    cout << "Elements: ";
    s.iterate();

    cout << "Contains 20: " << s.contains(20) << endl;

    s.remove(20);

    cout << "After removal: ";
    s.iterate();

    cout << "Size: " << s.size() << endl;

    s.clear();
    cout << "Size after clear: " << s.size() << endl;
}

/*
EXAMPLE WALKTHROUGH:

Assume:
    N = 10
    hash(x) = x % 10

----------------------------------
Initial:
    All buckets empty
    count = 0

----------------------------------
insert(10)
    hash = 10 % 10 = 0
    bucket[0] = [10]
    count = 1

----------------------------------
insert(20)
    hash = 20 % 10 = 0
    bucket[0] = [10, 20]
    count = 2

----------------------------------
insert(15)
    hash = 15 % 10 = 5
    bucket[5] = [15]
    count = 3

----------------------------------
insert(7)
    hash = 7 % 10 = 7
    bucket[7] = [7]
    count = 4

----------------------------------
insert(20)   // duplicate
    hash = 0
    bucket[0] already has 20
    → NOT inserted
    count = 4

----------------------------------
contains(15)
    hash = 5
    bucket[5] = [15]
    → found → return true

----------------------------------
contains(99)
    hash = 9
    bucket[9] = empty
    → not found → return false

----------------------------------
remove(20)

Before:
    bucket[0] = [10, 20]

After:
    bucket[0] = [10]
    count = 3

----------------------------------
iterate()

Traverse all buckets:
    bucket[0] → 10
    bucket[5] → 15
    bucket[7] → 7

Output:
    10 15 7   (order not guaranteed)

----------------------------------
clear()

    All buckets cleared
    count = 0

----------------------------------
FINAL STATE:
    empty set
*/