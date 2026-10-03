/*
Problem statement:
Given a system that has one event and multiple listeners, design and implement a mechanism for the listeners to hear about the event. 
We want to implement a callback mechanism that allows listeners to register.  
A function where are Callbacks registered before the event are executed when the event fires.

Solution:
This is a classic observer / publish–subscribe pattern problem.
You want multiple listeners to register callbacks, and when an event fires, all registered callbacks get invoked.

✅ Design Idea
Maintain a list of callbacks.
register_callback → adds a callback to the list.
event_fired → iterates over all callbacks and invokes them.

--------------------------

A callback is a function that you give to another function/system, so that it can call your function later when something happens.

Think of it as: "When this event happens, call this function."

Simple example

Suppose we have:
void hello() {
    cout << "Hello";
}

Normally, we call it ourselves:
hello();

But with a callback, we give hello to someone else:
Give hello() to the event system
             ↓
       wait for event
             ↓
       event happens
             ↓
       event system calls hello()

So the event system decides when to call it.

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
function<void()> cb
means: cb can hold any callable object that takes no arguments and returns void. 
*/