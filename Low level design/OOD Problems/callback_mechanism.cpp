/*
Problem statement:
Given a system that has one event and multiple listeners, 
design and implement a mechanism for the listeners to hear about the event. 
We want to implement a callback mechanism that allows listeners to register 
a function that will be invoked when the event fires. The API functions are register_callback and event_fired.

Solution:
This is a classic observer / publish–subscribe pattern problem.
You want multiple listeners to register callbacks, and when an event fires, all registered callbacks get invoked.

✅ Design Idea
Maintain a list of callbacks.
register_callback → adds a callback to the list.
event_fired → iterates over all callbacks and invokes them.

*/

#include <iostream>
#include <functional>
#include <vector>
using namespace std;

class EventSystem{
private:
    vector<function<void()>>callbacks;
public:
     // Register a callback
    void register_callback(function<void()> callback){
        callbacks.push_back(callback);
    }

    //Trigger update
    void event_fired(){
        for(auto &cb:callbacks){
            cb();  //invoke callback
        }
    }
};

void listener1(){
    cout<<"Listner1 raised an event"<<endl;
}
void listener2(){
    cout<<"Listner2 raised an event"<<endl;
}

int main(){
    EventSystem eventsystem;
    // Register functions
    eventsystem.register_callback(listener1);
    eventsystem.register_callback(listener2);
    
    // Fire event
    eventsystem.event_fired();
}

/*
  eventsystem.register_callback(listener1); Here, you are not calling the function, you are passing it.
  In void register_callback(std::function<void()> cb) , cb is a variable that can hold a function.
  
  std::function<void()> callback
👉 Means:
    You must pass something callable that behaves like a function with:no parameters ,no return value.
    🎯 So what is actually stored?
        👉 Not raw functions
        👉 But function wrappers (std::function) that can hold:

        Normal functions ✅,   Lambdas ✅,   Functors ✅

🧠 Why is std::function<void()> (callbacks) useful?
🎯 1. Enables Callbacks (Core Use Case)
    You don’t know what action to take, but you know when to trigger it.
    eventSystem.register_callback(listener1);
    The system says: “When event happens, I’ll call whatever you gave me.”

🔌 2. Decoupling (VERY important in design)
    Without callbacks:
    void event_fired() {
        listener1();
        listener2();
    }
    ❌ Hardcoded
    ❌ Not scalable

🧩 3. Extensibility
    You can add new behavior without modifying existing code.
    eventSystem.register_callback(newFeature);
    👉 No need to touch EventSystem.

*/