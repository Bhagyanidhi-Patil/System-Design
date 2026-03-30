#ifndef VEHICLEFACTORY_H
#define VEHICLEFACTORY_H

#include <string>
#include "Vehicle.h"

class VehicleFactory {
public:
    static Vehicle* createVehicle(const std::string& plate, VehicleType type) {
        if (plate.empty()) {
            return nullptr;
        }

        if (type == VehicleType::CAR) {
            return new Car(plate);
        } else if (type == VehicleType::BIKE) {
            return new Bike(plate);
        } else if (type == VehicleType::TRUCK) {
            return new Truck(plate);
        }

        return nullptr;
    }
};

#endif