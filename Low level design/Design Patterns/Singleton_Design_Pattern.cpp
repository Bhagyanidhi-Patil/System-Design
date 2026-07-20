/*
Singleton design pattern is creational design pattern. 
The Singleton Pattern is a creational design pattern that ensures a class has only one 
instance and provides a global point of access to that instance.

👉 Singleton = Only ONE object of a class can exist
No matter how many times you try to create it, you always get the same instance.

📌 Key Steps in Implementing Singleton
    1. Make the constructor private so that objects cannot be created directly from outside the class.
    2. Use a static member variable to store the single instance because a static member belongs to the class 
        and has only one copy throughout the program.
    3. Provide a public static getInstance() method that creates the object only if it doesn't already exist and 
        returns the same instance on every call. This gives global access to the single object.

             UML-Diagram

                Singleton
        +----------------------+
        | - instance: Singleton*|
        +----------------------+
        | - Singleton()        |
        +----------------------+
        | + getInstance()      |
        | + showMessage()      |
        +----------------------+

There are two types to initaize singleton instance : Eager and Lazy initalization.
    1. Eager iniatilzation: The instance is created when static variable is initalized , before main() starts.
        Singleton Singleton::instance = new Singleton();
    2.Lazy initalization: instance is created inside getinstance() only when first requested. Below code is lazy initalization.
    
    How to make your code "Thread-safety" ? 
    
    To make the Singleton pattern thread-safe, especially when dealing with lazy initialization, 
    we need to ensure that multiple threads do not create multiple instances of the singleton at the same time.
    Thread-Safety Strategies for Singleton:
	1. Mutex/Locking:
		○ Use a mutex to protect the critical section where the singleton instance is created.
		○ This approach ensures that only one thread can create the instance at a time.
	2. C++11 std::call_once (Recommended):
		○ The C++11 standard introduces std::call_once to execute a function only once, even across multiple threads. This makes the initialization thread-safe without explicit locking.

*/

#include <iostream>
using namespace std;
class Singleton{
private:
    static Singleton* instance;
    
    Singleton(){
        cout<<"Singleton created!"<<endl;
    }
public:
    static Singleton* getinstance(){
        if(instance==nullptr){
            instance = new Singleton();
        }
        return instance;
    }
    
    void showMessage(){
        cout<<"Hello from singleton"<<endl;
    }
};
Singleton* Singleton::instance = nullptr;

int main() {
    Singleton *s1 = Singleton::getinstance();
    Singleton* s2 = Singleton::getinstance();
    s1->showMessage();
    if(s1==s2){
        cout<<"Same instance"<<endl;
    }
    return 0;
}

//Thread safety code

#include <iostream>
#include<mutex>
using namespace std;

class Singleton{
private:
    static Singleton* instance;
    static mutex mtx;
    Singleton(){
        cout<<"Singleton created!"<<endl;
    }
public:
    static Singleton* getinstance(){
        lock_guard<mutex> lock(mtx);
        if(instance==nullptr){
            instance = new Singleton();
        }
        return instance;
    }
    
    void showMessage(){
        cout<<"Hello from singleton"<<endl;
    }
};
Singleton* Singleton::instance = nullptr;
mutex Singleton::mtx;

int main() {
    Singleton *s1 = Singleton::getinstance();
    Singleton* s2 = Singleton::getinstance();
    s1->showMessage();
    if(s1==s2){
        cout<<"Same instance"<<endl;
    }
    return 0;
}

/*
🧠 How It Works
    mutex mtx → ensures only one thread enters getInstance()
    lock_guard → automatically locks/unlocks
    Only one thread can create the object

    👉 When a thread enters the function and reaches this line:
            lock_guard<mutex> lock(mtx);

    🔒 Step 1: Lock
        The mutex is locked immediately
        If another thread already holds the lock → current thread waits

    🔓 Step 2: Unlock (Automatic)
        When the function (or scope) ends, lock_guard is destroyed
        Its destructor automatically unlocks the mutex
*/

//thread safe code (c++11 onwards)
//Only local static variables (inside a function) are thread-safe (since C++11)

#include <iostream>
using namespace std;

class Singleton{
private:
    Singleton(){
        cout<<"Singleton created"<<endl;
    }
public:
    static Singleton& getInstance(){
        static Singleton instance;   //thread -safe
        return instance;
    }
    void showMessage(){
        cout<<"Hello from singleton"<<endl;
    }
};
int main() {
    // Get instance first time
    Singleton& s1 = Singleton::getInstance();
    s1.showMessage();

    // Get instance second time
    Singleton& s2 = Singleton::getInstance();
    s2.showMessage();

    // Check if both are same
    if (&s1 == &s2) {
        cout << "Same instance\n";
    } else {
        cout << "Different instances\n";
    }

    return 0;
}
/*
Since C++11, the language guarantees that initialization of a function-local static variable is atomic and synchronized.
The compiler/runtime ensures that only one thread performs the initialization, while all other threads wait until it completes.

Memory Timeline: 

Initially ,No Singleton object

Thread 1
Creates Singleton

Static Memory
+-------------------+
| Singleton Object  |
+-------------------+

Thread 2
Object already exists
Return same object

Why is this guaranteed?
Because the C++11 standard says:
    Initialization of block-scope static variables is performed exactly once, even in the presence of multiple threads.
    The compiler automatically generates synchronization code (similar to using a mutex or std::call_once) around the local variable initialization.
    You don't write that synchronization yourself.
---
static Singleton instance; simply means:

only one copy of the variable/object exists,
it has static storage duration (lives for the lifetime of the program).

---
Approach 1: Modern C++11 (Meyers Singleton)

static Singleton& getInstance() {
    static Singleton instance;
    return instance;
}

Here we create an actual object:
static Singleton instance;

Memory:

Static Memory
+----------------------+
| Singleton instance   |
+----------------------+

    The object is stored in static storage.
    It is automatically created the first time getInstance() is called.
    It is automatically destroyed when the program exits.
    No new or delete is needed.

So we return a reference:
    return instance;
----
Approach 2: Older Singleton

class Singleton {
private:
    static Singleton* instance;
};

Notice this is only a pointer.

Initially:

instance
   |
   v
 nullptr

When getInstance() is called:

if(instance == nullptr)
    instance = new Singleton();

Now:

Static Memory

instance
   |
   v

Heap
+------------------+
| Singleton Object |
+------------------+

The object is created on the heap.

----

Why did older implementations use a pointer?

Suppose you wrote exactly the same code:

Singleton& getInstance() {
    static Singleton instance;
    return instance;
}

If two threads called getInstance() simultaneously, the C++ standard did not guarantee that only one thread would initialize instance.

So why did people use pointers?

They wanted lazy initialization, so they wrote:

class Singleton {
private:
    static Singleton* instance;

public:
    static Singleton* getInstance() {
        if (instance == nullptr)
            instance = new Singleton();

        return instance;
    }
};

This created the object only when it was first needed.
And for thread safety they used mutex. 
*/