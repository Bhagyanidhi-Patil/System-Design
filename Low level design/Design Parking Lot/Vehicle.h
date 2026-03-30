#ifndef VEHICLE_H
#define VEHICLE_H

#include<string>
using namespace std;

enum class VehicleType{
    BIKE,
    CAR,
    TRUCK
};

class Vehicle{
private:
    string licensePlate;
    VehicleType type;
public:
    Vehicle(string plate, VehicleType type);
    virtual ~Vehicle() {}
    VehicleType getType();
    string getPlate();
};

class Car : public Vehicle {
public:
    Car(const string& plate) : Vehicle(plate, VehicleType::CAR) {}
};

class Bike : public Vehicle {
public:
    Bike(const string& plate) : Vehicle(plate, VehicleType::BIKE) {}
};

class Truck : public Vehicle {
public:
    Truck(const string& plate) : Vehicle(plate, VehicleType::TRUCK) {}
};

#endif