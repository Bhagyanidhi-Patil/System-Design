// Implement a Set for values from 0 to N-1 with O(1) operations.

// #include <iostream>
// using namespace std;

// class Set{
// private:
//     bool *arr;
//     int size;
// public:
//     Set(int n){
//         size = n;
//         arr = new bool[size];
//         clear();
//     }

//     void insert(int x){
//         if(x>=0 && x<size)
//             arr[x] = true;
//     }

//     void remove(int x){
//         if(x>=0 && x<size)
//             arr[x] = false;
//     }

//     bool contains(int x){
//         if(x<0 || x>=size)return false;
//         return arr[x];
//     }

//     void clear(){
//         for(int i=0;i<size;i++){
//             arr[i] = false;
//         }
//     }

//     void iterate(){
//         for(int i=0;i<size;i++){
//             if(arr[i]){
//                 cout<<i<<" ";
//             }
//         }
//         cout<<endl;
//     }

//     ~Set(){
//         delete[] arr;
//     }
// };

// int main(){
//     Set s(10);   // Values allowed: 0 to 9

//     s.insert(2);
//     s.insert(5);
//     s.insert(7);

//     cout << s.contains(5) << endl;  // 1
//     cout << s.contains(3) << endl;  // 0

//     s.remove(5);

//     s.iterate();   // 2 7

//     s.clear();

//     s.iterate();   // Empty

//     return 0;
// }

//Can clear() be O(1)?

#include <iostream>
using namespace std;

class Set{
private:
    int *arr;
    int size;
    int currentVersion;
public:
    Set(int n){
        size = n;
        arr = new int[size];
        currentVersion = 1;
    }

    void insert(int x){
        if(x>=0 && x<size){
            arr[x] = currentVersion;
        }
    }

    void remove(int x){
        if(x>=0 && x<size){
            arr[x] = 0;
        }
    }

    void clear(){
        currentVersion++;
    }

    bool contains(int x){
        if(x<0 || x>=size)return false;
        return arr[x] == currentVersion;
    }

    void iterate(){
        for (int i = 0; i < size; i++) {
            if (arr[i] == currentVersion) {
                cout << i << " ";
            }
        }
        cout << endl;
    }

    ~Set(){
        delete []arr;
    }
};

int main(){
    Set s(10);   // Values allowed: 0 to 9

    s.insert(2);
    s.insert(5);
    s.insert(7);

    cout << s.contains(5) << endl;  // 1
    cout << s.contains(3) << endl;  // 0

    s.remove(5);

    s.iterate();   // 2 7

    s.clear();

    s.iterate();   // Empty

    return 0;
}

/*
What if values are not 0 ... N-1?

Our direct-array approach works only when the key range is known.

For example:

0 ... N-1

If values are:

100000
5000000
999999999

we can't reasonably create:

bool arr[1000000000];
because most positions would be unused.

Then we would typically use: hash table 
----
What is the memory cost?

For the simple boolean-array implementation:

bool* arr = new bool[N];
we need O(N) memory.
*/