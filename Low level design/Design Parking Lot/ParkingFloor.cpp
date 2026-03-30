#include "ParkingFloor.h"

ParkingFloor::ParkingFloor(int num):floorNo(num){}
ParkingSpot* ParkingFloor::isAvailableSpot(Vehicle *v){
    for(auto spot:spots){
        if(spot->isAvailable() && spot->canFitVehicle(v)){
            return spot;
        }
    }
    return nullptr;
}
void ParkingFloor::addSpot(ParkingSpot* spot){
    spots.push_back(spot);
}