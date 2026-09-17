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

    // Copy assignment
    sharedPtr& operator=(const sharedPtr& other) {
        if (this != &other) {
            (*refcount)--;

            if (*refcount == 0) {
                delete ptr;
                delete refcount;
            }

            ptr = other.ptr;
            refcount = other.refcount;
            (*refcount)++;
        }

        return *this;
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
        sharedPtr p2 = p1;   // Copy constructor
        sharedPtr p3(new A());

        p3 = p1;             // Copy assignment
    }

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

----
Copy constructor = creating a new object from an existing object
Copy assignment = assigning to an object that already exists

1. Copy constructor

p1 ─────► A
          ↑
          │
p2 ───────┘

refCount = 2

2. Copy assignment

SharedPtr p1(new A());
SharedPtr p2(new A());

p2 = p1;

Here, p2 already exists.
We're replacing what p2 currently owns with what p1 owns.

So this calls:
SharedPtr& operator=(const SharedPtr& other)

First, p2 must release its existing object:

Before:

p1 ─────► A       count = 1

p2 ─────► B       count = 1

After:

B → deleted

p1 ──┐
     ├────► A     count = 2
p2 ──┘

Simple summary

For:
p2 = p1;
this → p2
other → p1

Then:
Decrease p2's current reference count

(*refCount)--;

If p2 was the last owner (count == 0):
    delete ptr;
    delete refCount;

Make p2 point to the same object as p1
ptr = other.ptr;
refCount = other.refCount;

Increase the shared reference count because p2 is now also an owner:
(*refCount)++;
*/