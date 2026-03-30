# Parking Lot Design Requirements

## STEP 1: Clarify Requirements

### Functional Requirements:
- Vehicle types? Car, bike, scooty
- Entry and exit
- Multiple floors?
- Parking spot types? Small, medium, large
- Ticket generation?
- Fee calculation?

### 👉 Finalized version:
System should:
- Park a vehicle
- Remove a vehicle
- Assign appropriate spot
- Generate ticket
- Calculate parking fee on exit

## Step 2: Identify Core Entities
- Vehicle
- Parking spot
- Parking lot
- Payment
- Ticket
- Parking floor

## Step 3: Define Relationships
Think how objects interact:
- ParkingLot → has multiple → ParkingFloor
- ParkingFloor → has multiple → ParkingSpot
- ParkingSpot → holds → Vehicle
- Ticket → linked to → ParkingSpot
- Payment → uses → Ticket

## Step 4: Define Class Responsibilities

| Class | Responsibility |
|-------|---------------|
| Vehicle | Holds vehicle details |
| ParkingSpot | Check fit, park/remove vehicle |
| ParkingFloor | Find available spot |
| ParkingLot | Main controller |
| Ticket | Store entry info |
| Payment | Calculate fee |

## Step 5: Define Key Operations

### ParkingLot
- parkVehicle(Vehicle*)
- exitVehicle(ticketId)

### ParkingFloor
- findAvailableSpot(Vehicle*)

### ParkingSpot
- canFitVehicle()
- parkVehicle()
- removeVehicle()

## Step 6: Define Flow

### 🚗 Entry Flow
1. Vehicle arrives
2. ParkingLot searches floors
3. Floor finds available spot
4. Spot assigned
5. Ticket generated

### 🚪 Exit Flow
1. User provides ticket
2. System calculates fee
3. Spot freed
4. Ticket removed

## Step 7: Decide Data Structures

| Use Case | Data Structure |
|----------|----------------|
| Store floors | vector |
| Store spots | vector |
| Active tickets lookup | unordered_map |
| Fast nearest spot (advanced) | priority_queue |

## Step 8: Design Patterns that we can use:
- Factory Pattern → create vehicles
- Strategy Pattern → pricing logic
- Singleton → ParkingLot instance

## Step 9: Basic UML:

```
class ParkingSpot {
    - type
    - isOccupied
    + parkVehicle()
    + removeVehicle()
}

class Vehicle {
    - licensePlate
    - type
}

class Ticket {
    - id
    - entryTime
}
```

## Step 10: Entity Relationships (neat flow)

```
ParkingLot
  ├─ ParkingFloor
  │    └─ ParkingSpot
  │         └─ Vehicle
  ├─ Ticket
  ├─ Payment
  └─ Vehicle (tracked)
```

Relationships:
- ParkingLot -> ParkingFloor
- ParkingFloor -> ParkingSpot
- ParkingSpot -> Vehicle
- ParkingLot -> Ticket
- ParkingLot -> Payment
- ParkingLot -> Vehicle
