#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

class Coffee{
public:
    virtual string getName() = 0;
    virtual double getPrice() = 0;
    virtual ~Coffee(){}
};
class Espresso: public Coffee{
public:
    string getName() override{
        return "Espresso";
    }
    double getPrice() override{
        return 100;
    }
};

class Cappuccino: public Coffee{
public:
    string getName() override{
        return "Cappuccino";
    }
    double getPrice() override{
        return 120;
    }
};

class Latte: public Coffee{
public:
    string getName() override{
        return "Latte";
    }
    double getPrice() override{
        return 130;
    }
};

enum CoffeeType{
    ESPRESSO,
    CAPPUCCINO,
    LATTE
};

class CoffeeFactory{
public:
    static Coffee* createCoffee(CoffeeType type){
        switch(type){
        case CAPPUCCINO:
            return new Cappuccino();
        case LATTE:
            return new Latte();
        case ESPRESSO:
            return new Espresso();
        default: 
            return nullptr;
        }
    }
};

class CoffeeDecorator: public Coffee{
protected:
    Coffee* coffee;
public:
    CoffeeDecorator(Coffee* coffee){
        this->coffee = coffee;
    }
};

class Milk: public CoffeeDecorator{
public:
    Milk(Coffee* coffee):CoffeeDecorator(coffee){}
    string getName() override{
        return coffee->getName() + " Milk";
    }
    double getPrice() override{
        return coffee->getPrice() + 20;
    }
};

class Sugar: public CoffeeDecorator{
public:
    Sugar(Coffee* coffee):CoffeeDecorator(coffee){}
    string getName() override{
        return coffee->getName() + " Sugar";
    }
    double getPrice() override{
        return coffee->getPrice() + 10;
    }
};

class Payment{
public:
    virtual void pay(double amount) = 0;
    virtual ~Payment(){}
};

class CashPayment : public Payment{
public:
    void pay(double amount) override{
        cout<<"Paid "<<amount<<" using cash\n";
    }
};

class UPIPayment : public Payment{
public:
    void pay(double amount) override{
        cout<<"Paid "<<amount<<" using UPI\n";
    }
};

class CardPayment : public Payment{
public:
    void pay(double amount) override{
        cout<<"Paid "<<amount<<" using card\n";
    }
};

class PaymentStratergy{
private:
    Payment* stratergy;
public:
    PaymentStratergy(Payment *startergy){
        this->stratergy = startergy;
    }
    void payamount(double amount){
        stratergy->pay(amount);
    }
};

class Ingredient{
private:
    string name;
    int quantity;
public:
    Ingredient():name(""),quantity(0){}
    Ingredient(string name,int quantity):name(name),quantity(quantity){}

    bool available(int required){
        return quantity>=required;
    }
    void consume(int amount){
        quantity-=amount;
    }
};

class CoffeeMachine{
private:
    unordered_map<string,Ingredient>m;
public:
    CoffeeMachine():m{
        {"Coffee",Ingredient("Coffee",100)},
        {"Water",Ingredient("Water",100)},
        {"Milk",Ingredient("Milk",100)},
        {"Sugar",Ingredient("Sugar",100)},
    }
    {}

    void makeCoffee(CoffeeType type,Coffee* coffee,PaymentStratergy* payment){
        if(!m["Coffee"].available(10)|| !m["Water"].available(20)){
            cout<<"No enough ingredient\n";
            return;
        }
        if((type == CAPPUCCINO || type == LATTE) &&
           !m["Milk"].available(20)){

            cout << "Not enough milk\n";
            return;
        }
        payment->payamount(coffee->getPrice());

        m["Coffee"].consume(10);
        m["Water"].consume(20);

        if(type == CAPPUCCINO || type == LATTE){
            m["Milk"].consume(20);
        }

        cout << "Preparing " << coffee->getName() << "...\n";
        cout << "Coffee ready!\n";
    }
};

int main(){

    // Factory Pattern
    Coffee* coffee =
        CoffeeFactory::createCoffee(CAPPUCCINO);

    // Decorator Pattern
    coffee = new Milk(coffee);
    coffee = new Sugar(coffee);

    cout << "Coffee: " << coffee->getName() << endl;
    cout << "Price: " << coffee->getPrice() << endl;

    // Strategy Pattern
    Payment* payment = new UPIPayment();

    PaymentStratergy paymentStrategy(payment);

    // Coffee Machine
    CoffeeMachine machine;

    machine.makeCoffee(
        CAPPUCCINO,
        coffee,
        &paymentStrategy
    );

    return 0;
}