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
    Vehicle(string plate,VehicleType type);
    VehicleType getType();
    string getPlate();

};

#endif