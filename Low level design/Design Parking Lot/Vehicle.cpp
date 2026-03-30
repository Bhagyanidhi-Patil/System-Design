#include "Vehicle.h"

Vehicle::Vehicle(string plate,VehicleType type):licensePlate(plate),type(type){}
VehicleType Vehicle::getType(){
    return type;
}
string Vehicle::getPlate(){
    return licensePlate;
}