// Question : Design Unordered set 

#include<iostream>
#include<vector>
using namespace std;

class Unordered_set{
private:
    vector<bool>present;
public:
    Unordered_set(int n):present(n,false){}

    void insert(int x){
        if(x>=0 && x<present.size()){
            present[x] = true;
        }
    }
    void remove(int x){
        if(x>=0 && x<present.size()){
            present[x] = false;
        }
    }

    bool contains(int x){
        if(x>=0 && x<present.size()){
            return present[x];
        }
        return false;
    }
};

int main(){
    Unordered_set s(10);
    s.insert(3);
    s.insert(7);
    cout << s.contains(3) << "\n"; 
    cout << s.contains(5) << "\n"; 
    s.remove(3);
    cout << s.contains(3) << "\n"; 

}