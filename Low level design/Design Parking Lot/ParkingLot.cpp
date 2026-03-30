#include "ParkingLot.h"
#include "Payment.h"

ParkingLot* ParkingLot::instance = nullptr;

ParkingLot::ParkingLot() : ticketCounter(0) {}

ParkingLot* ParkingLot::getInstance() {
    if (!instance) {
        instance = new ParkingLot();
    }
    return instance;
}

void ParkingLot::addFloor(ParkingFloor* floor){
    floors.push_back(floor);
}

//Generate ticket at the time of parking 
Ticket* ParkingLot::parkVehicle(Vehicle *v){
    for(auto floor:floors){
        ParkingSpot* spot = floor->isAvailableSpot(v);
        if(spot && spot->canFitVehicle(v)){
            spot->parkVehicle(v);
            Ticket* ticket = new Ticket(++ticketCounter,spot);
            activeTickets[ticket->getId()] = ticket;
            return ticket;
        }
    }
    return nullptr;
}

//Payment is done at the time of exit
double ParkingLot::exitVehicle(int ticketId){
    if(activeTickets.find(ticketId)==activeTickets.end())
        return -1;
    Ticket* ticket = activeTickets[ticketId];
    double fee = Payment::calculateFee(ticket->getTime());
    ticket->getSpot()->removeVehicle();
    activeTickets.erase(ticketId);
    return fee;
}