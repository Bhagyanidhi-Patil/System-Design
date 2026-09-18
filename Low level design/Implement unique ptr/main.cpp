#include <iostream>
using namespace std;

class A{
public:
    A(){
        cout<<"A created\n";
    }
    ~A(){
        cout<<"A deleted\n";
    }
};

class UniquePtr{
    A* ptr;
public:
    //constructor
    UniquePtr(A* p=nullptr){
        ptr = p;
    }

    //copy constructor - not allowed
    UniquePtr(const UniquePtr&)=delete;

    //copy assignment - not allowed
    UniquePtr& operator=(const UniquePtr&) = delete;

    //move constructor
    UniquePtr(UniquePtr&& other){
        ptr = other.ptr;
        other.ptr = nullptr;
    }

    //move assignment
    UniquePtr& operator=(UniquePtr &&other){
        if(this!=&other){
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }
};

int main() {

    // -------------------------
    // Move Constructor
    // -------------------------

    UniquePtr p1(new A());

    UniquePtr p2 = move(p1);

    // p1 → nullptr
    // p2 → A


    // -------------------------
    // Move Assignment
    // -------------------------
    UniquePtr p4(new A());
    UniquePtr p3(new A());

    p3 = move(p4);

    // p4 → nullptr
    // p3 → A


    return 0;
}

/*
Move constructor

Before:
p1 ─────► A
p2 doesn't exist

After:
p1 ─────► nullptr

p2 ─────► A

Move assignment

UniquePtr p1(new A());
UniquePtr p2(new A());

p2 = std::move(p1);

Before:
p1 ─────► A
p2 ─────► B

After:
p1 ─────► nullptr

p2 ─────► A

*/