/*
callbacks can be registered before an event. When the event fires, registered callbacks execute. 
A callback registered after the event may need to execute immediately, depending on the specified semantics.

register cb1
register cb2

event_fired()
→ cb1()
→ cb2()

register cb3
→ cb3() immediately

register cb4
→ cb4() immediately
*/

#include <iostream>
#include <functional>
#include <vector>
using namespace std;

class EventManager{
private:
    vector<function<void()>>callbacks;
    bool eventfired = false;
public:
    void register_callback(function<void()>cb){
        if(eventfired)
            cb();
        else    
            callbacks.push_back(cb);
    }

    void event_fired(){
        if(eventfired)return ;   //to make sure we don't fire the same event twice.

        eventfired = true;
        for(auto &cb:callbacks){
            cb();
        }
        callbacks.clear();
    }
};

int main() {

    EventManager eventManager;

    // Register callbacks before the event
    eventManager.register_callback([]() {
        cout << "Callback 1 executed" << endl;
    });

    eventManager.register_callback([]() {
        cout << "Callback 2 executed" << endl;
    });

    // Fire the event
    cout << "Event fired!" << endl;
    eventManager.event_fired();

    // Register callback after the event
    eventManager.register_callback([]() {
        cout << "Callback 3 executed immediately" << endl;
    });

    return 0;
}

/*
This is a lamba function
eventManager.register_callback([]() {
    cout << "Callback 1 executed" << endl;
});

[capture](parameters) -> return_type {
    // body
};

Part	  Meaning
[]	      Capture list
()	      No parameters
{ ... }	  Function body

the capture list [] is related to how a lambda accesses variables from the surrounding scope, 
including whether it captures them by value or by reference.
[x]     → x by value
[&x]    → x by reference
[=]     → used outside variables by value
[&]     → used outside variables by reference
[]      → capture nothing

*/