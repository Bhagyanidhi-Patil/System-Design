/*
register cb1
event_fired()
→ cb1()

register cb2
event_fired()
→ cb1()
→ cb2()
*/

#include <iostream>
#include <functional>
#include <vector>
using namespace std;

class EventManager{
private:
    vector<function<void()>>callbacks;
public:
    void register_callback(function<void()>cb){
        callbacks.push_back(cb);
    }
    void eventfired(){
        for(auto &cb:callbacks){
            cb();
        }
    }
};
void function1(){
    cout<<"Callback 1 executed"<<endl;
}
void function2(){
    cout<<"Callback 2 executed"<<endl;
}
int main(){
    EventManager eventmanager;
    // eventmanager.register_callback([](){
    //     cout<<"Callback 1 executed"<<endl;
    // });
    // eventmanager.eventfired();
    // cout << "Event fired!" << endl;
    // eventmanager.register_callback([](){
    //     cout<<"Callback 2 executed"<<endl;
    // });
    eventmanager.register_callback(function1);
    eventmanager.eventfired();
    cout << "Event fired!" << endl;
    eventmanager.register_callback(function2);
    eventmanager.eventfired();
    cout << "Event fired!" << endl;
    return 0;
}