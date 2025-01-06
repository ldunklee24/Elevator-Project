#ifndef ECElevatorConnect_h
#define ECElevatorConnect_h

#include "ECElevatorObserver.h"
#include <vector>
#include <string>
#include <fstream>

// Connection between simulation and visualization
// Purpose: Bridges simulation data with visual representation

// PassengerInfo struct (Lines 9-16)
// Purpose: Data structure for passenger information from input file

struct PassengerInfo {
    int arrivalTime;
    int startFloor;
    int destFloor;
    
    PassengerInfo(int time, int start, int dest) 
        : arrivalTime(time), startFloor(start), destFloor(dest) {}
};

class ECElevatorObserver;

class ECElevatorConnect {
public:
    ECElevatorConnect(const std::string& filename, ECElevatorObserver* observer);
    void LoadSimulation();
    void Update(int currentTime);
    bool HasMorePassengers() const;
    int GetTotalTime() const { return totalTime; }
    int GetNumFloors() const { return numFloors; }
    int GetTotalPassengers() const { return totalPassengers; }
    int GetDeliveredPassengers() const { return deliveredPassengers; }
    void IncrementDeliveredPassengers() { deliveredPassengers++; }
    
private:
    void AddPassenger(const PassengerInfo& passenger);
    
    std::string filename;
    int numFloors;
    int totalTime;
    int totalPassengers;
    int deliveredPassengers;
    std::vector<PassengerInfo> passengers;
    size_t nextPassengerIndex;
    ECElevatorObserver* elevatorObserver;
};

// ECElevatorConnect Class (Lines 20-43)
// Purpose: Manages simulation data and state
// Key components:
// - Loads passenger data from file
// - Tracks simulation progress
// - Updates observer with new passengers
// - Manages passenger statistics

#endif
