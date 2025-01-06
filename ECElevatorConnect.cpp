#include "ECElevatorConnect.h"
#include "ECElevatorObserver.h"
#include <algorithm>
#include <sstream>
#include <iostream>

ECElevatorConnect::ECElevatorConnect(const std::string& fname, ECElevatorObserver* observer) 
    : filename(fname), nextPassengerIndex(0), elevatorObserver(observer),
      totalPassengers(0), deliveredPassengers(0) {}

void ECElevatorConnect::LoadSimulation() {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::string line;
    
    // Skip comment lines
    while (std::getline(file, line)) {
        if (line.empty() || line[0] != '#') {
            break;
        }
    }
    
    // Parse first non-comment line for floors and time
    std::stringstream ss(line);
    ss >> numFloors >> totalTime;
    
    // Read passenger information
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        int time, start, dest;
        std::stringstream ss(line);
        ss >> time >> start >> dest;
        passengers.push_back(PassengerInfo(time, start, dest));
        totalPassengers++;
    }
    
    // Sort passengers by arrival time
    std::sort(passengers.begin(), passengers.end(),
              [](const PassengerInfo& a, const PassengerInfo& b) {
                  return a.arrivalTime < b.arrivalTime;
              });
}

void ECElevatorConnect::Update(int currentTime) {
    while (nextPassengerIndex < passengers.size() && 
           passengers[nextPassengerIndex].arrivalTime <= currentTime) {
        AddPassenger(passengers[nextPassengerIndex]);
        nextPassengerIndex++;
    }
}

void ECElevatorConnect::AddPassenger(const PassengerInfo& passenger) {
    // Call the appropriate method in ECElevatorObserver to add a passenger
    // This will depend on your ECElevatorObserver implementation
    // For example:
    if (passenger.destFloor > passenger.startFloor) {
        // Going up
        elevatorObserver->AddPassenger(passenger.startFloor, passenger.destFloor, true);
    } else {
        // Going down
        elevatorObserver->AddPassenger(passenger.startFloor, passenger.destFloor, false);
    }
}

bool ECElevatorConnect::HasMorePassengers() const {
    return nextPassengerIndex < passengers.size();
}