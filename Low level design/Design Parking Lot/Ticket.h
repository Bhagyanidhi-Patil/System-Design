#ifndef TICKET_H
#define TICKET_H
#include "ParkingSpot.h"
#include <ctime>

class Ticket{
private:
    int id;
    time_t entryTime;
    ParkingSpot* spot;
public:
    Ticket(int id,ParkingSpot* spot);
    int getId();
    time_t getTime();
    ParkingSpot* getSpot();
};

#endif