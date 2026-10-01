/*
A Set is a data structure that stores a collection of unique elements in sorted order.
An unordered_set stores unique elements but does not maintain sorted order.

If the values are restricted to 0...N-1, I can use a direct-address array. 
If the values are arbitrary and the range is large or unknown, I would use a hash table for average O(1) operations. 
Even if values are restricted 0...N-1 and value of N is too large then hash table is good option.
If ordered iteration is required, I would use a balanced BST.
*/
#include <iostream>
#include <list>
using namespace std;

class Set{
private:
    static const int size = 10;
    list<int> table[size];
    int hashfunction(int x){
        return x%size;
    }
public:
    void insert(int x){
        int index = hashfunction(x);

        for(int value:table[index]){
            if(value == x)return;
        }
        table[index].push_back(x);
    }

    bool contains(int x){
        int index = hashfunction(x);
        for(int value:table[index]){
            if(value == x)return true;
        }
        return false;
    }

    void remove(int x){
        int index = hashfunction(x);
        for(auto it= table[index].begin();it!=table[index].end();++it){
            if(*it==x){
                table[index].erase(it);
                return;
            }
        }
    }

    void clear(){
        for(int i=0;i<size;i++){
            table[i].clear();
        }
    }

    void iterate(){
        for(int i=0;i<size;i++){
            for(int value:table[i]){
                cout<<value<<" ";
            }
        }
        cout<<endl;
    }
};

int main() {

    Set s;

    s.insert(10);
    s.insert(25);
    s.insert(15);
    s.insert(10);  // duplicate → ignored

    cout << s.contains(25) << endl; // 1
    cout << s.contains(50) << endl; // 0

    s.remove(25);

    s.iterate();

    s.clear();

    return 0;
}