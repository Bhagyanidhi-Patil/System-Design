/*
Singleton design pattern is creational design pattern. 
The Singleton Pattern is a creational design pattern that ensures a class has only one 
instance and provides a global point of access to that instance.

👉 Singleton = Only ONE object of a class can exist
No matter how many times you try to create it, you always get the same instance.

📌 Key Steps in Implementing Singleton
	1. Private Constructor → Prevents direct instantiation.
	2. Static Member Variable → Holds the single instance of the class.
	3. Public getInstance() Method → Provides global access to the instance.

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
    static Singleton *instance;
    
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
    static Singleton *instance;
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