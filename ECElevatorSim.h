//
//  ECElevatorSim.h
//  
//
//  Created by Yufeng Wu on 6/27/23.
//  Elevator simulation

#ifndef ECElevatorSim_h
#define ECElevatorSim_h

#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <string>
#include <queue>
#include <algorithm>

using namespace std;
//*****************************************************************************
// DON'T CHANGE THIS CLASS
// 
// Elevator simulation request: 
// (i) time: when the request is made
// (ii) floorSrc: which floor the user is at at present
// (iii) floorDest floor: where the user wants to go; we assume floorDest != floorSrc
// 
// Note: a request is in three stages:
// (i) floor request: the passenger is waiting at floorSrc; once the elevator arrived 
// at the floor (and in the right direction), move to the next stage
// (ii) inside request: passenger now requests to go to a specific floor once inside the elevator
// (iii) Once the passenger arrives at the floor, this request is considered to be "serviced"
//
// two sspecial requests:
// (a) maintenance start: floorSrc=floorDest=-1; put elevator into maintenance 
// starting at the specified time; elevator starts at the current floor
// (b) maintenance end: floorSrc=floorDest=0; put elevator back to operation (from the current floor)

class ECElevatorSimRequest
{
public:
    ECElevatorSimRequest(int timeIn, int floorSrcIn, int floorDestIn) : time(timeIn), floorSrc(floorSrcIn), floorDest(floorDestIn), fFloorReqDone(false), fServiced(false), timeArrive(-1) {} 
    ECElevatorSimRequest(const ECElevatorSimRequest &rhs) : time(rhs.time), floorSrc(rhs.floorSrc), floorDest(rhs.floorDest), fFloorReqDone(rhs.fFloorReqDone), fServiced(rhs.fServiced), timeArrive(rhs.timeArrive) {}
    int GetTime() const {return time; }
    int GetFloorSrc() const { return floorSrc; }
    int GetFloorDest() const { return floorDest; }
    bool IsGoingUp() const { return floorDest >= floorSrc; }

    // Is this passenger in the elevator or not
    bool IsFloorRequestDone() const { return fFloorReqDone; }
    void SetFloorRequestDone(bool f) { fFloorReqDone = f; }

    // Is this event serviced (i.e., the passenger has arrived at the desstination)?
    bool IsServiced() const { return fServiced; }
    void SetServiced(bool f) { fServiced = f; }

    // Get the floor to service
    // If this is in stage (i): waiting at a floor, return that floor waiting at
    // If this is in stage (ii): inside an elevator, return the floor going to
    // Otherwise, return -1
    int GetRequestedFloor() const {
        if( IsServiced() )  {
            return -1;
        }
        else if( IsFloorRequestDone() )   {
            return GetFloorDest();
        }
        else {
            return GetFloorSrc();
        }
    }

    // Wait time: get/set. Note: you need to maintain the wait time yourself!
    int GetArriveTime() const { return timeArrive; }
    void SetArriveTime(int t) { timeArrive = t; }

    // Check if this is the special maintenance start request
    bool IsMaintenanceStart() const { return floorSrc==-1 && floorDest==-1; }
    bool IsMaintenanceEnd() const { return floorSrc==0 && floorDest==0; }

private:
    int time;           // time of request made
    int floorSrc;       // which floor the request is made
    int floorDest;      // which floor is going
    bool fFloorReqDone;   // is this passenger passing stage one (no longer waiting at the floor) or not
    bool fServiced;     // is this request serviced already?
    int timeArrive;     // when the user gets to the desitnation floor
};

//*****************************************************************************
// Elevator moving direction

typedef enum
{
    EC_ELEVATOR_STOPPED = 0,    // not moving
    EC_ELEVATOR_UP,             // moving up
    EC_ELEVATOR_DOWN            // moving down
} EC_ELEVATOR_DIR;

//*****************************************************************************
// Add your own classes here...

// Abstract base class for elevator simulation
class ECElevatorSimBase {
public:
    virtual ~ECElevatorSimBase() = default;
    
    // Pure virtual functions
    virtual void ProcessFloorRequests(int currentTime) = 0;
    virtual void MoveElevator(int currentTime) = 0;
    
    // Virtual functions with default implementation
    virtual int GetNumFloors() const { return numFloors; }
    virtual int GetCurrFloor() const { return currFloor; }
    virtual void SetCurrFloor(int f) { currFloor = f; }
    virtual EC_ELEVATOR_DIR GetCurrDir() const { return currDir; }
    virtual void SetCurrDir(EC_ELEVATOR_DIR dir) { currDir = dir; }

protected:
    ECElevatorSimBase(int numFloors, std::vector<ECElevatorSimRequest> &listRequests) 
        : numFloors(numFloors), listRequests(listRequests), currFloor(1), currDir(EC_ELEVATOR_STOPPED) {}
    
    int numFloors;
    int currFloor;
    EC_ELEVATOR_DIR currDir;
    std::vector<ECElevatorSimRequest> &listRequests;
};

// Concrete implementation class
class ECElevatorSim : public ECElevatorSimBase {
public:
    ECElevatorSim(int numFloors, std::vector<ECElevatorSimRequest> &listRequests) 
        : ECElevatorSimBase(numFloors, listRequests), isMoving(false), waitTime(0) {}
    
    void Simulate(int lenSim) {
        for (int time = 0; time < lenSim; time++) {
            // Process requests first
            ProcessFloorRequests(time);
            
            // If we're waiting, continue waiting
            if (waitTime > 0) {
                waitTime--;
                continue;
            }
            
            // Move elevator if needed
            MoveElevator(time);
        }
    }
    
    void ProcessFloorRequests(int time) override {
        bool processedRequest = false;
        
        // Process all requests at current floor
        for (auto &request : listRequests) {
            if (request.GetTime() > time || request.IsServiced()) {
                continue;
            }

            // Handle pickup
            if (!request.IsFloorRequestDone() && request.GetFloorSrc() == currFloor) {
                request.SetFloorRequestDone(true);
                processedRequest = true;
            }

            // Handle dropoff
            if (request.IsFloorRequestDone() && !request.IsServiced() && 
                request.GetFloorDest() == currFloor) {
                request.SetServiced(true);
                request.SetArriveTime(time);
                processedRequest = true;
            }
        }

        // Set wait time only once for all requests at this floor
        if (processedRequest && isMoving) {
            waitTime = 1;
            isMoving = false;
        }
    }
    
    void MoveElevator(int time) override {
        int nextFloor = GetNextDestination(time);
        if (nextFloor == -1) {
            currDir = EC_ELEVATOR_STOPPED;
            isMoving = false;
            return;
        }

        // Start moving if not already moving
        if (!isMoving) {
            if (nextFloor > currFloor) {
                currDir = EC_ELEVATOR_UP;
            } else if (nextFloor < currFloor) {
                currDir = EC_ELEVATOR_DOWN;
            }
            isMoving = true;
        }

        // Move one floor
        if (currDir == EC_ELEVATOR_UP) {
            currFloor++;
        } else if (currDir == EC_ELEVATOR_DOWN) {
            currFloor--;
        }
    }

private:
    bool isMoving;
    int waitTime;

    int GetNextDestination(int time) {
        int nextFloor = -1;
        int minDistance = numFloors + 1;

        // First handle requests in current direction
        for (const auto &request : listRequests) {
            if (request.GetTime() > time) continue;

            int targetFloor = request.GetRequestedFloor();
            if (targetFloor == -1) continue;

            if (currDir == EC_ELEVATOR_UP && targetFloor >= currFloor) {
                if (nextFloor == -1 || targetFloor < nextFloor) {
                    nextFloor = targetFloor;
                }
            } else if (currDir == EC_ELEVATOR_DOWN && targetFloor <= currFloor) {
                if (nextFloor == -1 || targetFloor > nextFloor) {
                    nextFloor = targetFloor;
                }
            }
        }

        // If no requests in current direction, find closest request
        if (nextFloor == -1) {
            for (const auto &request : listRequests) {
                if (request.GetTime() > time) continue;

                int targetFloor = request.GetRequestedFloor();
                if (targetFloor == -1) continue;

                int distance = abs(targetFloor - currFloor);
                if (distance < minDistance || (distance == minDistance && targetFloor > currFloor)) {
                    minDistance = distance;
                    nextFloor = targetFloor;
                }
            }
        }

        return nextFloor;
    }
};

#endif /* ECElevatorSim_h */