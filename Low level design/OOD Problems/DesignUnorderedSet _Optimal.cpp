//Implement unordered set in optimal way


#include <iostream>
using namespace std;

class UnorderedSet {
public:
    static const int N = 1009;
    int arr[N];
    int count;

    UnorderedSet() {
        count = 0;
    }

    // INSERT (no duplicates) , TC O(N) because we used lookup, O(1)if look up is not there 
    void insert(int x) {
        if (lookUp(x)) return; // avoid duplicate

        if (count < N) {
            arr[count] = x;
            count++;
        }
    }

    // REMOVE , TC O(N)
    void remove(int x) {
        for (int i = 0; i < count; i++) {
            if (arr[i] == x) {
                arr[i] = arr[count - 1]; // move last element here
                count--;                // shrink size
                return;
            }
        }
    }

    // LOOKUP , TC  O(N)
    bool lookUp(int x) {
        for (int i = 0; i < count; i++) {
            if (arr[i] == x) return true;
        }
        return false;
    }

    // CLEAR , TC O(1)
    void clearAll() {
        count = 0;
    }

    // ITERATE , TC O(N)
    //  f → function pointer
    // (*f) → “f is a pointer to a function”
    // (int) → function takes an int parameter
    // void (before (*f)) → function returns void
    void iterate(void (*f)(int)) {
        for (int i = 0; i < count; i++) {
            f(arr[i]); // pass value, not index
        }
    }
};

// Example function
void print(int x) {
    cout << x << " ";
}

int main() {
    UnorderedSet s;

    s.insert(10);
    s.insert(20);
    s.insert(30);
    s.insert(10); // ignored

    s.iterate(print);
    cout << endl;

    s.remove(20);
    s.iterate(print);
    cout << endl;

    cout << s.lookUp(10) << endl;

    s.clearAll();
    cout << "Size after clear: " << s.count << endl;
}

/*
EXAMPLE TRACE:

Initial:
    arr = []
    count = 0

----------------------------------
insert(10)
    arr = [10]
    count = 1

insert(20)
    arr = [10, 20]
    count = 2

insert(30)
    arr = [10, 20, 30]
    count = 3

insert(20)  // duplicate
    lookUp(20) → true
    → NOT inserted

----------------------------------
lookUp(10)
    checks: 10 → found
    returns true

lookUp(50)
    checks all → not found
    returns false

----------------------------------
remove(20)

Before:
    arr = [10, 20, 30]
    count = 3

Step:
    found at index = 1
    arr[1] = arr[count-1] = arr[2] = 30
    count--

After:
    arr = [10, 30, 30]  // last value ignored
    count = 2

Valid elements:
    [10, 30]

----------------------------------
iterate(print)

Loop:
    i = 0 → print 10
    i = 1 → print 30

Output:
    10 30

----------------------------------
clearAll()

    count = 0

Now:
    set is empty
*/