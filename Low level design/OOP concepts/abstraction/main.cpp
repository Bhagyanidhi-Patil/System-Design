#include <iostream>
using namespace std;

class CoffeeMachine {
private:
    void boilWater() {
        cout << "Boiling Water..." << endl;
    }

    void addCoffee() {
        cout << "Adding Coffee Powder..." << endl;
    }

    void addMilk() {
        cout << "Adding Milk..." << endl;
    }
public:
    void makeCoffee() {
        boilWater();
        addCoffee();
        addMilk();
        cout << "Coffee is Ready!" << endl;
    }
};

int main() {

    CoffeeMachine coffee;

    coffee.makeCoffee();

}

/*
"In this example, the user only calls makeCoffee(). 
They don't need to know that internally the coffee machine boils water, 
adds coffee powder, and adds milk. These implementation details are hidden 
inside private methods, while only the essential functionality is exposed 
through the public method makeCoffee(). This is abstraction."

Q. "Is this really abstraction, or is it encapsulation?"
-> It demonstrates both.
    Abstraction
        The user only sees makeCoffee().
        Internal steps are hidden.
    Encapsulation
        The data and methods are bundled inside the CoffeeMachine class.
        Helper methods (boilWater(), addCoffee(), addMilk()) are made private, so they cannot be called directly from outside.

*/