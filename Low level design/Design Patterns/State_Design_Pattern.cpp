/*
The State Design Pattern is a behavioral design pattern that allows an object to change its behavior when its internal state changes.
💡 Key Idea
    Instead of using lots of if-else or switch statements, you:
    -> Create separate state classes
    -> Move behavior into those classes
    -> Let the object switch between them
📦 Example (Real-life)
    Think of a mobile phone:
        When it's locked, pressing buttons does nothing
        When it's unlocked, buttons perform actions

    Same button → different behavior → depending on state

🧩 Components
    1.Context - The main object whose behavior changes
    2.State (Interface) - Declares common behavior
    3.Concrete States - Implement behavior for each state

                     UML-Diagram
                +------------------+
                |      State       |  <<interface>>
                +------------------+
                | +handle()        |
                +------------------+
                        ▲
                ┌───────┼────────┐
                │       │        │
        +---------------+ +---------------+ +---------------+
        |   RedState    | |  GreenState   | | YellowState   |
        +---------------+ +---------------+ +---------------+
        | +handle()     | | +handle()     | | +handle()     |
        +---------------+ +---------------+ +---------------+

                        |
                        |
                +----------------------+
                |     TrafficLight     |  (Context)
                +----------------------+
                | - currentState       |
                +----------------------+
                | +setState(State*)    |
                | +request()           |
                +----------------------+
                        |
                        v
                        uses
*/
#include <iostream>
using namespace std;

class TrafficLight;

//State Interface
class State{
public:
    virtual void handle(TrafficLight* light) = 0;
    virtual ~State() {}
};

//Concrete States
class RedState : public State {
public:
    void handle(TrafficLight* light) override;
};
class GreenState : public State {
public:
    void handle(TrafficLight* light) override;
};
class YellowState : public State {
public:
    void handle(TrafficLight* light) override;
};

//Context
class TrafficLight{
private:
    State* state;
public:
    TrafficLight() {
        state = new RedState();  // Initial state
    }
    void setState(State* newState){ 
        state = newState; 
    }
    void request(){
        state->handle(this);
    }
};

void RedState::handle(TrafficLight* light){
    cout<<"Red Light - Stop!"<<endl;
    light->setState(new GreenState());
}

void GreenState::handle(TrafficLight* light){
    cout<<"Green Light - Go!"<<endl;
    light->setState(new YellowState());
}

void YellowState::handle(TrafficLight* light){
    cout<<"Yellow Light - Caution!"<<endl;
    light->setState(new RedState());
}

int main(){
    TrafficLight light;
    for(int i=0;i<6;i++){
        light.request();
    }
    return 0;
}