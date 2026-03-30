#ifndef PARKINGSPOT_H
#define PARKINGSPOT_H
#include "Vehicle.h"

enum class SpotType{
    SMALL,
    MEDIUM,
    LARGE
};

class ParkingSpot{
private:
    SpotType type;
    Vehicle* vehicle;
    int id;
    bool isOccupied;
public:
    ParkingSpot(SpotType type,int id);
    bool isAvailable();
    void removeVehicle();
    bool canFitVehicle(Vehicle *v);
    bool parkVehicle(Vehicle *v);
};

#endif