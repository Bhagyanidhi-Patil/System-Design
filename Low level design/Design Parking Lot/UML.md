# Parking Lot UML Diagram (Updated with Design Patterns)

# Parking Lot UML Diagram (Updated with Design Patterns)

```
Core Classes:
+-------------------+       +---------------------+       +---------------------+
|    ParkingLot     |       |    ParkingFloor     |       |    ParkingSpot      |
|  (Singleton)      |       |                     |       |                     |
| - floors: vector  | ----> | - floorNo: int      | ----> | - type: SpotType    |
| - activeTickets   |       | - spots: vector     |       | - vehicle: Vehicle* |
| - ticketCounter   |       |                     |       | - id: int           |
|                   |       | + isAvailableSpot() |       | - isOccupied: bool  |
| + getInstance()   |       | + addSpot()         |       | + isAvailable()     |
| + parkVehicle()   |       +---------------------+       | + canFitVehicle()   |
| + exitVehicle()   |               |                     | + parkVehicle()     |
+-------------------+               |                     | + removeVehicle()   |
          |                         |                     +---------------------+
          | has multiple            | has multiple                    |
          v                         v                                 |
+-------------------+       +---------------------+                   |
|     Ticket        |       |    Payment           |                  |
|                   |       |  (Strategy Context)  |                  |
| - id: int         |       | - strategy: PricingStrategy* |          |
| - spot: ParkingSpot*| --> |                      |                  |
| - entryTime       |       | + setStrategy()      |                  |
|                   |       | + calculateFee()     |                  |
| + getId()         |       +---------------------+                   |
| + getTime()       |               |                                 |
| + getSpot()       |               | uses                            |
+-------------------+               v                                 |
                                    +---------------------+           |
                                    | PricingStrategy     |           |
                                    |  (Strategy Interface)|          |
                                    |                     |           |
                                    | + calculateFee()    |           |
                                    +---------------------+           |
                                              ^                       |
                                              | inherits              |
                                              v                       |
+-------------------+       +---------------------+       +---------------------+
| VehicleFactory    |       |  HourlyPricing      |       |  FlatRatePricing    |
|  (Factory)        |       |  (Concrete Strategy)|       |  (Concrete Strategy)|
|                   |       |                     |       |                     |
| + createVehicle() |       | + calculateFee()    |       | + calculateFee()    |
+-------------------+       +---------------------+       +---------------------+
          | creates
          v
+-------------------+
|      Vehicle      |
|                   |
| - licensePlate    |
| - type: VehicleType|
|                   |
| + getType()       |
| + getPlate()      |
+-------------------+

Enums:
- VehicleType: BIKE, CAR, TRUCK
- SpotType: SMALL, MEDIUM, LARGE

Relationships:
- ParkingLot --> ParkingFloor (aggregation)
- ParkingFloor --> ParkingSpot (aggregation)
- ParkingLot --> Ticket (manages)
- Ticket --> ParkingSpot (references; spot contains vehicle)
- ParkingSpot --> Vehicle (contains)
- ParkingLot --> Payment (uses)
- Payment --> PricingStrategy (uses)
- PricingStrategy <|-- HourlyPricing (inheritance)
- PricingStrategy <|-- FlatRatePricing (inheritance)
- VehicleFactory --> Vehicle (creates)

Design Notes:
- Ticket references a ParkingSpot, which itself contains the Vehicle.  
- There is no direct Ticket->Vehicle link; relationship is indirect through ParkingSpot.

Design Patterns Used:
1. **Singleton Pattern**: ParkingLot class ensures only one instance exists (getInstance() method).
2. **Factory Pattern**: VehicleFactory class centralizes Vehicle creation.
3. **Strategy Pattern**: PricingStrategy interface allows pluggable fee calculations (HourlyPricing, FlatRatePricing).
```