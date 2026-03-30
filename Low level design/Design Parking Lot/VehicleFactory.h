#ifndef VEHICLEFACTORY_H
#define VEHICLEFACTORY_H

#include <string>
#include "Vehicle.h"

class VehicleFactory {
public:
    static Vehicle* createVehicle(const std::string& plate, VehicleType type) {
        // Validation can be added here (non-empty plate etc.)
        return new Vehicle(plate, type);
    }
};

#endif