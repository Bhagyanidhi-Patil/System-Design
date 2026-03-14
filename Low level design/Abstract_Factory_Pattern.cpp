/*
• Abstract factory method is a factory of a factory pattern(pattern inside a pattern).
• It is creational design pattern which is required for creating objects which belong to a family of similar objects. 
Client
   |
   v
VehicleFactory
   |
   +-------------------+
   |                   |
ToyotaFactory     HondaFactory
   |                   |
   v                   v
ToyotaCar          HondaCar
ToyotaBike         HondaBike
*/

#include <iostream>
using namespace std;

//interface class / Abstract class
class car{
public:
    virtual void assemble() = 0;
    virtual ~car(){}
};

class bike{
public:
    virtual void assemble() = 0;
    virtual ~bike(){}
};

//concrete class
class toyotaCar:public car{
public:
    void assemble() override{
        cout << "Assembling Toyota Car" << endl;
    }
};
class toyotaBike : public bike {
public:
    void assemble() override {
        cout << "Assembling Toyota Bike" << endl;
    }
};

class hondaCar : public car {
public:
    void assemble() override {
        cout << "Assembling Honda Car" << endl;
    }
};
class hondaBike : public bike {
public:
    void assemble() override {
        cout << "Assembling Honda Bike" << endl;
    }
};

//Abstract Factory 
class vechileFactory{
public:
    virtual car* createCar() =0;
    virtual bike* createBike() = 0;
    virtual ~vechileFactory() {}
};

//Concrete Factories
class toyotaFactory:public vechileFactory{
public:
    car* createCar() override{
        return new toyotaCar();
    }
    bike* createBike() override{
        return new toyotaBike();
    }
};
class hondaFactory:public vechileFactory{
public:
    car* createCar() override{
        return new hondaCar();
    }
    bike* createBike() override{
        return new hondaBike();
    }
};

//Client code
int main(){
    vechileFactory* factory = new toyotaFactory();
    car* car = factory->createCar();
    bike* bike = factory->createBike();

    car->assemble();
    bike->assemble();

    delete car;
    delete bike;
    delete factory;

    return 0;
}





