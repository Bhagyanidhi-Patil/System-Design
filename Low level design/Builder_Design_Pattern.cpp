/*
The Builder Design Pattern is a creational design pattern used to construct complex objects step by step.
👉 Instead of creating a complex object using:
    ->a large constructor with many parameters, or
    ->multiple overloaded constructors for different combinations
👉 The Builder pattern allows you to:
    ->construct the object step by step
    ->separate construction logic from the object’s representation
    ->create different representations using the same construction process

🏗️ Structure
    Product – the complex object being built
    Builder – interface for building parts
    Concrete Builder – actual implementation
    Director (optional) – controls construction order

                    UML-Diagram

                -----------------------
                |   Client            |
                ----------------------
                          │ uses
                          ▼  
                ┌──────────────────────┐
                │       Director       │
                ├──────────────────────┤
                │ + construct()        │
                └─────────┬────────────┘
                          │ uses
                          ▼
                ┌──────────────────────┐
                │    CarBuilder        │  <<interface>>
                ├──────────────────────┤
                │ + buildEngine()     │
                │ + buildWheels()     │
                │ + buildSunroof()    │
                │ + buildGPS()        │
                │ + getCar()          │
                └─────────┬────────────┘
                          ▲
        ┌─────────────────┴─────────────────┐
        │                                   │
┌──────────────────────┐         ┌──────────────────────┐
│  SportsCarBuilder    │         │  EconomyCarBuilder   │
├──────────────────────┤         ├──────────────────────┤
│ - car : Car          │         │ - car : Car          │
├──────────────────────┤         ├──────────────────────┤
│ + buildEngine()      │         │ + buildEngine()      │
│ + buildWheels()      │         │ + buildWheels()      │
│ + buildSunroof()     │         │ + buildSunroof()     │
│ + buildGPS()         │         │ + buildGPS()         │
│ + getCar()           │         │ + getCar()           │
└─────────┬────────────┘         └─────────┬────────────┘
          │                                  │
          │ creates                          │ creates
          ▼                                  ▼
                ┌──────────────────────┐
                │         Car          │
                ├──────────────────────┤
                │ - engine             │
                │ - wheels             │
                │ - sunroof            │
                │ - gps                │
                ├──────────────────────┤
                │ + setEngine()        │
                │ + setWheels()        │
                │ + setSunroof()       │
                │ + setGPS()           │
                │ + show()             │
                └──────────────────────┘
*/

#include<iostream>
#include<string>
using namespace std;

//🚗 Product
class Car{
private:
    string engine;  
    int wheels;     
    bool sunroof;
    bool gps;
public:
    Car():wheels(0),sunroof(false),gps(false){}

    void setEngine(const string& eng){engine = eng;}
    void setWheels(int w){wheels = w;}
    void setSunroof(bool s){sunroof = s;}
    void setGps(bool g){gps = g;}

    void show() const{
        cout << "Engine: " << engine
             << ", Wheels: " << wheels
             << ", Sunroof: " << (sunroof ? "Yes" : "No")
             << ", GPS: " << (gps ? "Yes" : "No")
             << endl;
    }
};

//🔨 Builder Interface
class CarBuilder{
public:
    virtual void buildEngine() = 0;
    virtual void buildWheels() = 0;
    virtual void buildSunroof() = 0;
    virtual void buildGPS() = 0;

    virtual Car* getCar() = 0;

    virtual ~CarBuilder() {}
};

//🏎️ Concrete Builder
class SportsCarBuilder : public CarBuilder {
private:
    Car* car;

public:
    SportsCarBuilder() {
        car = new Car();
    }

    void buildEngine() override {
        car->setEngine("V8 Engine");
    }

    void buildWheels() override {
        car->setWheels(4);
    }

    void buildSunroof() override {
        car->setSunroof(true); // optional included
    }

    void buildGPS() override {
        car->setGps(true);
    }

    Car* getCar() override {
        return car;
    }
};

class EconomyCarBuilder : public CarBuilder {
private:
    Car* car;

public:
    EconomyCarBuilder() {
        car = new Car();
    }

    void buildEngine() override {
        car->setEngine("V4 Engine");
    }

    void buildWheels() override {
        car->setWheels(4);
    }

    void buildSunroof() override {
        car->setSunroof(false); // optional excluded
    }

    void buildGPS() override {
        car->setGps(false);
    }

    Car* getCar() override {
        return car;
    }
};

//🎬 Director
class Director {
public:
    void construct(CarBuilder* builder) {
        builder->buildEngine();   // mandatory
        builder->buildWheels();   // mandatory
        builder->buildSunroof();  // optional
        builder->buildGPS();      // optional
    }
};

//🚀 Client Code
int main() {
    Director director;

    // Build Sports Car
    CarBuilder* sportsBuilder = new SportsCarBuilder();
    director.construct(sportsBuilder);

    Car* sportsCar = sportsBuilder->getCar();
    sportsCar->show();

    // Build Economy Car
    CarBuilder* ecoBuilder = new EconomyCarBuilder();
    director.construct(ecoBuilder);

    Car* ecoCar = ecoBuilder->getCar();
    ecoCar->show();

    // cleanup
    delete sportsCar;
    delete sportsBuilder;
    delete ecoCar;
    delete ecoBuilder;

    return 0;
}