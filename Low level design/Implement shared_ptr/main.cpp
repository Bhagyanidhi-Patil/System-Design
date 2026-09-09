#include <iostream>
using namespace std;

class A {
public:
    int data;

    A() {
        cout << "A object created\n";
    }

    ~A() {
        cout << "A object deleted\n";
    }
};

class sharedPtr {
    A* ptr;
    int* refcount;

public:
    // Constructor
    sharedPtr(A* p) {
        ptr = p;
        refcount = new int(1);

        cout << "sharedPtr created. Refcount = "
             << *refcount << endl;
    }

    // Copy constructor
    sharedPtr(const sharedPtr& other) {
        ptr = other.ptr;
        refcount = other.refcount;

        (*refcount)++;

        cout << "sharedPtr copied. Refcount = "
             << *refcount << endl;
    }

    // Destructor
    ~sharedPtr() {
        (*refcount)--;

        cout << "sharedPtr destroyed. Refcount = "
             << *refcount << endl;

        if (*refcount == 0) {
            cout << "Refcount is 0. Deleting object...\n";

            delete ptr;
            delete refcount;
        }
    }
};

int main() {

    sharedPtr p1(new A());

    {
        sharedPtr p2 = p1;

        cout << "Inside scope\n";
    }

    cout << "p2 destroyed, back in main\n";

    return 0;
}