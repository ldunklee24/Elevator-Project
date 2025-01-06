#include "ECElevatorObserver.h"
#include <cmath>
#include <algorithm>
#include <sstream>

ECElevatorObserver::ECElevatorObserver(ECGraphicViewImp* view) 
    : graphicView(view), currentFloor(1), numPassengers(0), 
      isMovingUp(false), isMoving(false), 
      currentPosition((NUM_FLOORS - 1) * FLOOR_HEIGHT),
      shouldStopAtNext(false), isAutomatic(true), stopTimer(0), 
      isPaused(false), currentTime(0) {
    
    // Initialize button states
    for (int i = 0; i < NUM_FLOORS; i++) {
        upButtons[i] = false;
        downButtons[i] = false;
    }
}

ECElevatorObserver::~ECElevatorObserver() {
    // Empty destructor
}

void ECElevatorObserver::Update() {
    if (!graphicView) return;

    ECGVEventType event = graphicView->GetCurrEvent();
    
    // Handle spacebar for pause/resume
    if (event == ECGV_EV_KEY_UP_SPACE) {
        isPaused = !isPaused;
        return;
    }
    
    // Don't process other updates if paused
    if (isPaused) {
        DrawElevator();
        DrawFloorButtons();
        DrawPassengerCount();
        DrawTimeBar();
        DrawWaitingPassengers();
        graphicView->SetRedraw(true);
        return;
    }

    // Only update on timer events when not paused
    if (event == ECGV_EV_TIMER) {
        static int timerCount = 0;
        if (++timerCount >= 37) {
            currentTime++;
            timerCount = 0;
        }
        
        // Move elevator if not paused and there are requests
        if (!buttonQueue.empty()) {
            MoveElevator();
        }
        
        ProcessNewPassengers();
    }
    
    DrawElevator();
    DrawFloorButtons();
    DrawPassengerCount();
    DrawTimeBar();
    DrawWaitingPassengers();
    
    graphicView->SetRedraw(true);
}

void ECElevatorObserver::MoveElevator() {
    if (stopTimer > 0) {
        stopTimer--;
        return;
    }

    float moveSpeed = 2.0f;
    
    if (!isMoving && !buttonQueue.empty()) {
        int targetFloor = buttonQueue.front();
        float targetPosition = (NUM_FLOORS - 1 - targetFloor) * FLOOR_HEIGHT;
        isMovingUp = currentPosition > targetPosition;
        isMoving = true;
    }
    
    if (isMoving) {
        float targetPosition = (NUM_FLOORS - 1 - buttonQueue.front()) * FLOOR_HEIGHT;
        
        if (isMovingUp) {
            currentPosition = std::max(currentPosition - moveSpeed, targetPosition);
        } else {
            currentPosition = std::min(currentPosition + moveSpeed, targetPosition);
        }
        
        // Check if we've reached the target floor
        if (std::abs(currentPosition - targetPosition) < moveSpeed) {
            currentPosition = targetPosition;
            currentFloor = buttonQueue.front();
            isMoving = false;
            
            // Process passengers
            ProcessPassengers();
            
            // Remove this floor from queue
            if (!buttonQueue.empty()) {
                buttonQueue.erase(buttonQueue.begin());
            }
            
            // Reset floor buttons
            upButtons[currentFloor] = false;
            downButtons[currentFloor] = false;
            
            stopTimer = STOP_DURATION;
        }
    }
}

void ECElevatorObserver::ProcessPassengers() {
    // Pick up waiting passengers
    auto waitIt = waitingPassengers.begin();
    while (waitIt != waitingPassengers.end()) {
        if (waitIt->startFloor == currentFloor) {
            passengers.push_back(*waitIt);
            numPassengers++;
            
            // Add destination to queue if not already there
            if (std::find(buttonQueue.begin(), buttonQueue.end(), waitIt->destFloor) 
                == buttonQueue.end()) {
                buttonQueue.push_back(waitIt->destFloor);
            }
            waitIt = waitingPassengers.erase(waitIt);
        } else {
            ++waitIt;
        }
    }
    
    // Drop off passengers
    auto it = passengers.begin();
    while (it != passengers.end()) {
        if (it->destFloor == currentFloor) {
            numPassengers--;
            if (simulator) {
                simulator->IncrementDeliveredPassengers();
            }
            it = passengers.erase(it);
        } else {
            ++it;
        }
    }
}

void ECElevatorObserver::DrawElevator() {
    if (!graphicView) return;
    
    // Draw shaft
    graphicView->DrawRectangle(
        LEFT_MARGIN, 
        0, 
        LEFT_MARGIN + ELEVATOR_WIDTH + 20, 
        NUM_FLOORS * FLOOR_HEIGHT,
        2,
        ECGV_BLACK
    );
    
    // Draw horizontal lines between floors
    for (int floor = 0; floor < NUM_FLOORS; floor++) {
        int y = floor * FLOOR_HEIGHT;
        graphicView->DrawLine(
            LEFT_MARGIN - 80,  // Extend a bit to the left of the buttons
            y,
            LEFT_MARGIN + ELEVATOR_WIDTH + 100,  // Extend a bit past the elevator
            y,
            ECGV_BLACK
        );
    }
    
    // Draw elevator cabin
    graphicView->DrawFilledRectangle(
        LEFT_MARGIN + 10,
        currentPosition,
        LEFT_MARGIN + ELEVATOR_WIDTH + 10,
        currentPosition + ELEVATOR_HEIGHT,
        ECGV_BLUE
    );
    
    // Draw passengers and their destination indicators
    int passengerX = LEFT_MARGIN + 20;
    int passengerY = currentPosition + 20;
    
    for (const auto& passenger : passengers) {
        // Draw passenger rectangle
        graphicView->DrawFilledRectangle(
            passengerX,
            passengerY,
            passengerX + PASSENGER_WIDTH,
            passengerY + PASSENGER_HEIGHT,
            passenger.color
        );
        
        // Draw destination dots inside passenger rectangle
        for(int i = 0; i < passenger.destFloor; i++) {
            graphicView->DrawFilledCircle(
                passengerX + PASSENGER_WIDTH/2,
                passengerY + 5 + (i * 5),
                2,
                ECGV_BLACK
            );
        }
        
        passengerX += PASSENGER_WIDTH + 10;
    }
}

void ECElevatorObserver::DrawFloorButtons() {
    if (!graphicView) return;
    
    for (int floor = 0; floor < NUM_FLOORS; floor++) {
        int y = (NUM_FLOORS - 1 - floor) * FLOOR_HEIGHT + FLOOR_HEIGHT/2;
        
        // Skip drawing floor number text
        
        // Draw buttons
        ECGVColor upColor = upButtons[floor] ? ECGV_RED : ECGV_BLACK;
        ECGVColor downColor = downButtons[floor] ? ECGV_RED : ECGV_BLACK;
        
        graphicView->DrawFilledCircle(
            LEFT_MARGIN - 30,
            y - 15,
            BUTTON_SIZE/2,
            upColor
        );
        
        graphicView->DrawFilledCircle(
            LEFT_MARGIN - 30,
            y + 15,
            BUTTON_SIZE/2,
            downColor
        );
    }
}

void ECElevatorObserver::DrawPassengerCount() {
    if (!graphicView) return;
    
    // Draw passenger count as rectangles instead of text
    for (int i = 0; i < numPassengers; i++) {
        graphicView->DrawFilledRectangle(
            LEFT_MARGIN + ELEVATOR_WIDTH + 50 + (i * 15),
            30,
            LEFT_MARGIN + ELEVATOR_WIDTH + 60 + (i * 15),
            40,
            ECGV_GREEN
        );
    }
    
    // Draw pause indicator as a red rectangle if paused
    if (isPaused) {
        graphicView->DrawFilledRectangle(
            LEFT_MARGIN + ELEVATOR_WIDTH + 50,
            60,
            LEFT_MARGIN + ELEVATOR_WIDTH + 90,
            80,
            ECGV_RED
        );
    }
}

void ECElevatorObserver::AddPassenger(int startFloor, int destFloor, bool goingUp) {
    static int nextId = 0;
    
    // Create passenger with random color (avoiding blue)
    ECGVColor colors[] = {ECGV_RED, ECGV_GREEN, ECGV_YELLOW, ECGV_CYAN, ECGV_PURPLE};
    ECGVColor randomColor = colors[nextId % 5];  // Changed from 5 to 4 since array has 4 colors
    
    Passenger newPassenger(nextId++, startFloor, destFloor, currentTime, randomColor);
    
    // Set appropriate button
    if (goingUp) {
        upButtons[startFloor] = true;
    } else {
        downButtons[startFloor] = true;
    }
    
    waitingPassengers.push_back(newPassenger);
    
    if (std::find(buttonQueue.begin(), buttonQueue.end(), startFloor) == buttonQueue.end()) {
        buttonQueue.push_back(startFloor);
    }
}

void ECElevatorObserver::ProcessNewPassengers() {
    // Check for new passengers at the current time
    if (simulator) {
        simulator->Update(currentTime);
    }
}

void ECElevatorObserver::DrawTimeBar() {
    // Draw outline
    graphicView->DrawRectangle(TIME_BAR_X, TIME_BAR_Y, 
                              TIME_BAR_X + TIME_BAR_WIDTH, 
                              TIME_BAR_Y + TIME_BAR_HEIGHT, 
                              ECGV_BLACK);
    
    // Calculate progress based on delivered passengers
    int totalPassengers = simulator ? simulator->GetTotalPassengers() : 0;
    int deliveredPassengers = simulator ? simulator->GetDeliveredPassengers() : 0;
    
    if (totalPassengers > 0) {
        float progress = static_cast<float>(deliveredPassengers) / totalPassengers;
        int filledWidth = static_cast<int>(TIME_BAR_WIDTH * progress);
        
        // Draw filled portion
        graphicView->DrawFilledRectangle(TIME_BAR_X, TIME_BAR_Y,
                                       TIME_BAR_X + filledWidth,
                                       TIME_BAR_Y + TIME_BAR_HEIGHT,
                                       ECGV_GREEN);
    }
}

void ECElevatorObserver::DrawWaitingPassengers() {
    if (!graphicView) return;
    
    for (const auto& passenger : waitingPassengers) {
        int y = (NUM_FLOORS - 1 - passenger.startFloor) * FLOOR_HEIGHT + FLOOR_HEIGHT/2;
        
        // Draw passenger
        graphicView->DrawFilledRectangle(
            LEFT_MARGIN - 60,
            y - 15,
            LEFT_MARGIN - 45,
            y,
            passenger.color
        );
        
        // Draw direction arrow
        bool goingUp = passenger.destFloor > passenger.startFloor;
        int arrowY = y + (goingUp ? -20 : 5);
        int arrowTipY = arrowY + (goingUp ? -5 : 5);
        
        graphicView->DrawLine(
            LEFT_MARGIN - 52,
            arrowY,
            LEFT_MARGIN - 52,
            arrowTipY,
            ECGV_BLACK
        );

        // Draw destination indicator: small dots showing how many floors to move
        int numFloors = abs(passenger.destFloor - passenger.startFloor);
        for(int i = 0; i < numFloors; i++) {
            graphicView->DrawFilledCircle(
                LEFT_MARGIN - 75,
                y - 10 + (i * 5),
                2,
                ECGV_BLACK
            );
        }
    }
}