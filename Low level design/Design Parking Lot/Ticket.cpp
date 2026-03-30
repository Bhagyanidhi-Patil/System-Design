#include "Ticket.h"

Ticket::Ticket(int id,ParkingSpot* spot):id(id),spot(spot){
    entryTime = time(nullptr);
}
int Ticket::getId(){
    return id;
}
time_t Ticket::getTime(){
    return entryTime;
}
ParkingSpot* Ticket::getSpot(){
    return spot;
}