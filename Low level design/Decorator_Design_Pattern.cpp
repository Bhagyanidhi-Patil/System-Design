/*
The Decorator Design Pattern is a structural pattern that lets you add new functionality 
to an object dynamically (at runtime) without modifying its existing code.
🔹 Key Idea
    You have a base component interface
    • A concrete component
    • A decorator base class (that also implements the same interface)
    • Multiple concrete decorators that add behavior

	Why Use the Decorator Pattern?
	• Without the Decorator Pattern, if you wanted to add new features to an object, 
    you would have to write different combinations of classes to accommodate every possible feature combination. 
    For example:
	• If you wanted to add milk and sugar to your coffee, you'd need to create new classes like MilkCoffee, SugarCoffee, 
    MilkSugarCoffee, and so on. This approach can lead to an explosion of classes, making your codebase harder to maintain and extend.

                    UML-Diagram
                    
                    <<interface>>
                     Coffee
        -----------------------------------
        + getDescription() : string
        + cost() : double
        -----------------------------------
                               ▲
       (is-a)                  │
        ----------------------------------- (is-a , has-a)
        |                                 |
+----------------------+      +------------------------------+
|    SimpleCoffee      |      |     CoffeeDecorator          |
+----------------------+      +------------------------------+
|                      |      | - coffee : Coffee*           |
+----------------------+      +------------------------------+
| + getDescription()   |      | + getDescription()           |
| + cost()             |      | + cost()                     |
+----------------------+      +------------------------------+
                                         ▲
                                         │  (is-a)
                       -------------------------------------
                       |                                   |
         +---------------------------+     +---------------------------+
         |      MilkDecorator        |     |     SugarDecorator        |
         +---------------------------+     +---------------------------+
         |                           |     |                           |
         +---------------------------+     +---------------------------+
         | + getDescription()        |     | + getDescription()        |
         | + cost()                  |     | + cost()                  |
         +---------------------------+     +---------------------------+
*/

#include<iostream>
using namespace std;

//Component Interface
class Coffee{
public:
    virtual string getDescription()=0;
    virtual double cost()=0;
    virtual ~Coffee(){}
};
//Concrete Component
class SimpleCoffee:public Coffee{
public:
    string getDescription(){
        return "Simple Coffee";
    }
    double cost(){
        return 5.0;
    }
};

//Decorator Base Class ( CoffeeDecorator 'is-a' and 'has-a' Coffee)
class CoffeeDecorator:public Coffee{
protected:
    Coffee* coffee;
public:
    CoffeeDecorator(Coffee *c){
        coffee = c;
    }
};
//Concrete Decorators
class MilkDecorator:public CoffeeDecorator{
public:
    MilkDecorator(Coffee *c):CoffeeDecorator(c){}
    string getDescription(){
        return coffee->getDescription()+" + Milk";
    }
    double cost(){
        return coffee->cost()+3.0;
    }
};
class SugarDecorator:public CoffeeDecorator{
public:
    SugarDecorator(Coffee *c):CoffeeDecorator(c){}
    string getDescription(){
        return coffee->getDescription()+" + Sugar";
    }
    double cost(){
        return coffee->cost()+2.0;
    }
};

int main(){
    Coffee *coffee = new SimpleCoffee();
    cout<<"Before adding decorator"<<endl;
    cout<<coffee->getDescription()<<endl;
    cout<<"Cost: "<<coffee->cost()<<endl;
    cout<<"After adding decorator"<<endl;
    coffee = new MilkDecorator(coffee);
    coffee = new SugarDecorator(coffee);

    cout<<coffee->getDescription()<<endl;
    cout<<"Cost: "<<coffee->cost()<<endl;
}