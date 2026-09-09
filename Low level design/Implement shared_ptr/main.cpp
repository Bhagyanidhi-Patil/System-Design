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
    int* refcount;        //We keep refcount as a pointer because all copies of sharedPtr must share the same reference count.

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


/*
Suppose refcount was just an integer
class sharedPtr {
    A* ptr;
    int refcount;
};

When you do:

sharedPtr p1(new A());  // refcount = 1
sharedPtr p2 = p1;

p2 gets its own copy of refcount.

p1
 ├── ptr ──────► A object
 └── refcount = 1

p2
 ├── ptr ──────► Same A object
 └── refcount = 2

The counts are stored separately, so updating one doesn't update the other.

----

With int* refcount
A* ptr;
int* refcount;

When p1 is created:

ptr = new A();
refcount = new int(1);

Memory looks like:

p1
 ├── ptr ───────────► A object
 │
 └── refcount ──────► [ 1 ]

Now when:

sharedPtr p2 = p1;

we do:

ptr = other.ptr;
refcount = other.refcount;

(*refcount)++;

Both objects now point to the same integer:

p1 ──┐
     ├──► A object
p2 ──┘


p1.refcount ──┐
              ├──► [ 2 ]
p2.refcount ──┘

Now both p1 and p2 see the same reference count.



*/