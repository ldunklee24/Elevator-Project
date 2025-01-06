#ifndef ECElevatorObserver_h
#define ECElevatorObserver_h

#include "ECObserver.h"
#include "ECGraphicViewImp.h"
#include "ECElevatorConnect.h"
#include <string>
#include <vector>
#include <map>

class ECElevatorConnect;
class ECGraphicViewImp;

struct Passenger {
    Passenger(int id, int start, int dest, int startTime, ECGVColor col) 
        : id(id), startFloor(start), destFloor(dest), startTime(startTime), color(col) {}
    
    int id;
    int startFloor;
    int destFloor;
    int startTime;
    ECGVColor color;
};

class ECElevatorObserver : public ECObserver {
public:
    ECElevatorObserver(ECGraphicViewImp* view);
    virtual ~ECElevatorObserver();
    
    virtual void Update() override;
    virtual void AddPassenger(int startFloor, int destFloor, bool goingUp);
    void SetCurrentTime(int time) { currentTime = time; }
    void TogglePause() { isPaused = !isPaused; }
    void SetSimulator(ECElevatorConnect* sim) { simulator = sim; }

protected:
    std::vector<Passenger> waitingPassengers;
    std::vector<int> buttonQueue;
    std::map<int, bool> upButtons;    
    std::map<int, bool> downButtons;
    int currentTime;

private:
    void DrawElevator();
    void DrawFloorButtons();
    void DrawPassengerCount();
    void MoveElevator();
    void ProcessPassengers();
    void DrawText(int x, int y, const std::string& text, ECGVColor color);
    void ProcessNewPassengers();
    void DrawTimeBar();
    void DrawWaitingPassengers();
    
    ECGraphicViewImp* graphicView;
    std::vector<Passenger> passengers;
    
    // Elevator state
    int currentFloor;
    int numPassengers;
    bool isMovingUp;
    bool isMoving;
    float currentPosition;
    bool isPaused;
    bool isAutomatic;
    bool shouldStopAtNext;
    int stopTimer;
    
    // Constants
    static const int NUM_FLOORS = 10;
    static const int FLOOR_HEIGHT = 60;
    static const int ELEVATOR_WIDTH = 60;
    static const int ELEVATOR_HEIGHT = 60;
    static const int LEFT_MARGIN = 150;
    static const int BUTTON_SIZE = 8;
    static const int PASSENGER_WIDTH = 15;
    static const int PASSENGER_HEIGHT = 20;
    static const int STOP_DURATION = 50;
    static const int TIME_BAR_WIDTH = 200;
    static const int TIME_BAR_HEIGHT = 20;
    static const int TIME_BAR_X = 400;
    static const int TIME_BAR_Y = 30;
    
    ECElevatorConnect* simulator = nullptr;
};

#endif
