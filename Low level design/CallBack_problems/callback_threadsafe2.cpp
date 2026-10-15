//Every callback must execute exactly once, and callbacks registered after the event fires must execute immediately
#include <iostream>
#include <functional>
#include <vector>
#include <mutex>
using namespace std;

class EventManager{
private:
    vector<function<void()>>callback;
    mutex mtx;
    bool event_fired = false;
public:
    void register_callback(function<void()>cb){
        bool executenow = false;
        {
        lock_guard<mutex>lock(mtx);
        if(event_fired == true)
            executenow = true;
        else
            callback.push_back(cb);
        }
        if(executenow)
            cb();
    }

    void eventfired(){
        vector<function<void()>>currcallbacks;
        {
            lock_guard<mutex>lock(mtx);
            if(event_fired==true)return;
            event_fired = true;
            currcallbacks.swap(callback);   //here every callback must execute exactly once, so swap not copy
        }
        for(auto &cb:currcallbacks){
            cb();
        }
    }
};

int main() {

    EventManager eventManager;

    eventManager.register_callback([]() {
        cout << "Callback 1 executed" << endl;
    });

    eventManager.register_callback([]() {
        cout << "Callback 2 executed" << endl;
    });

    cout << "Event fired!" << endl;

    eventManager.eventfired();

    return 0;
}

