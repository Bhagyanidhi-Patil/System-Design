#ifndef PARKINGFLOOR_H
#define PARKINGFLOOR_H
#include<vector>
#include "ParkingSpot.h"
#include "Vehicle.h"

class ParkingFloor{
private:
    int floorNo;
    vector<ParkingSpot*>spots;
public:
    ParkingFloor(int num);
    void addSpot(ParkingSpot* spot);
    ParkingSpot* isAvailableSpot(Vehicle *v);
};


#endif