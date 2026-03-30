#include <iostream>
#include "ParkingLot.h"
#include "VehicleFactory.h"
#include "Payment.h"
#include "HourlyPricing.h"
#include "FlatRatePricing.h"

using namespace std;

int main() {
    ParkingLot* lot = ParkingLot::getInstance();

    Payment::setStrategy(new HourlyPricing());

    ParkingFloor* floor1 = new ParkingFloor(1);
    floor1->addSpot(new ParkingSpot(SpotType::SMALL, 1));
    floor1->addSpot(new ParkingSpot(SpotType::MEDIUM, 2));
    floor1->addSpot(new ParkingSpot(SpotType::LARGE, 3));

    lot->addFloor(floor1);

    Vehicle* car = VehicleFactory::createVehicle("KA01AB1234", VehicleType::CAR);

    Ticket* ticket = lot->parkVehicle(car);

    if (ticket) {
        cout << "Parked! Ticket ID: " << ticket->getId() << endl;
    } else {
        cout << "No space available!" << endl;
    }

    // Simulate exit
    double fee = lot->exitVehicle(ticket->getId());
    cout << "Parking Fee: Rs " << fee << endl;

    return 0;
}