#include "ParkingSpot.h"

ParkingSpot::ParkingSpot(SpotType type, int id) : id(id), type(type), isOccupied(false), vehicle(nullptr) {}
bool ParkingSpot::canFitVehicle(Vehicle *v){
    if(type == SpotType::SMALL && v->getType() == VehicleType::BIKE)
        return true;
    else if(type == SpotType::MEDIUM && v->getType() == VehicleType::CAR)
        return true;
    else if(type == SpotType::LARGE && v->getType() == VehicleType::TRUCK)
        return true;
    return false;
}
bool ParkingSpot::isAvailable(){
    return !isOccupied;
}
bool ParkingSpot::parkVehicle(Vehicle *v){
    if(!isOccupied && canFitVehicle(v)){
        isOccupied = true;
        vehicle = v;
        return true;
    }
    return false;
}
void ParkingSpot::removeVehicle(){
    isOccupied = false;
    vehicle = nullptr;
}