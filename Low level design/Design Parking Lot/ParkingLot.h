#ifndef PARKINGLOT_H
#define PARKINGLOT_H
#include "ParkingFloor.h"
#include "Ticket.h"
#include <unordered_map>
#include<vector>

class ParkingLot{
private:
    static ParkingLot* instance;
    ParkingLot();
    ParkingLot(const ParkingLot&) = delete;
    ParkingLot& operator=(const ParkingLot&) = delete;

    vector<ParkingFloor*> floors;
    unordered_map<int, Ticket*> activeTickets;
    int ticketCounter;
public:
    static ParkingLot* getInstance();
    void addFloor(ParkingFloor *floor);
    Ticket* parkVehicle(Vehicle *v);
    double exitVehicle(int ticketId);
};
#endif

//Parking lot is implemented as a singleton class to ensure only one instance of parking lot exists in the system. 
//It manages multiple parking floors and keeps track of active tickets for parked vehicles. The parkVehicle method checks for available spots across all floors and generates a ticket if parking is successful. The exitVehicle method calculates the parking fee based on the entry time and removes the vehicle from the spot.                                                                              