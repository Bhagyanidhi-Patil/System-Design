/*
The Observer Design Pattern is a behavioural design pattern that defines a 
one-to-many dependency between objects so that when one object (the Subject) 
changes state, all its dependent objects (the Observers) are notified and updated automatically.

The Observer pattern is like a YouTube subscription system:
    A channel (subject) uploads a new video
    All subscribers (observers) get notified automatically
    👉 So instead of checking manually, observers are automatically informed when something changes.

    Subject (Publisher)
        Holds data
        Keeps a list of observers
        Notifies them when data changes

    Observer (Subscriber)
        Wants to know when something changes
        Has an update() function

                 UML - Diagram

                <<interface>>
                Observer
            +----------------------+
            | + update(int temp)   |
            +----------------------+
                        ▲
                ---------------------
                |                   |
        +-------------------+  +-------------------+
        |  PhoneDisplay     |  |    TVDisplay      |
        +-------------------+  +-------------------+
        | + update(int)     |  | + update(int)     |
        +-------------------+  +-------------------+


                WeatherStation
            +---------------------------+
            | - observers: vector<Obs*> |
            | - temperature: int        |
            +---------------------------+
            | + addObserver(Observer*)  |
            | + setTemperature(int)     |
            | + notify()                |
            +---------------------------+
                        |
                        |  (1..*)
                        |
                        ▼
                    Observer
*/


#include <iostream>
#include <vector>
using namespace std;

//Observer Interface
class Observer{
public:
    virtual void update(int temperature)=0;
};
//Concrete Observers
class PhoneDisplay:public Observer{
public:
    void update(int temperature)override{
        cout<<"Phone display: Temperature updated to "<<temperature<<endl;
    }
};
class TVDisplay:public Observer{
public:
    void update(int temperature)override{
        cout<<"TV display: Temperature updated to "<<temperature<<endl;
    }
};

//Subject Class
class WeatherStation{
private:
    vector<Observer*>Observers;
    int temperature;
public:
    void addObservers(Observer *obs){
        Observers.push_back(obs);
    }
    void setTemperature(int t){
        temperature = t;
        notify();
    }
    void notify(){
        for(auto obs:Observers){
            obs->update(temperature);
        }
    }
};


int main() {
    WeatherStation station;
    // PhoneDisplay phone;
    // TVDisplay tv;
    
    station.addObservers(new PhoneDisplay()); 
    station.addObservers(new TVDisplay());
    //station.addObservers(&phone); 
    //station.addObservers(&tv);
    station.setTemperature(25);
    station.setTemperature(30);
    
    return 0;
}

