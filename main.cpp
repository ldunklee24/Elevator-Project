#include "ECElevatorConnect.h"
#include <iostream>

// Add this before main()
class ConcreteElevatorObserver : public ECElevatorObserver {
public:
    ConcreteElevatorObserver(ECGraphicViewImp* view) : ECElevatorObserver(view) {}
    
    // You can remove the AddPassenger override if you want to use the base class implementation
    // Or keep it if you want custom behavior
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <simulation_file>" << std::endl;
        return 1;
    }

    try {
        // Initialize graphic view first
        ECGraphicViewImp* graphicView = new ECGraphicViewImp(800, 600);
        if (!graphicView) {
            throw std::runtime_error("Failed to create graphic view");
        }

        // Create observer and simulator
        ConcreteElevatorObserver* elevatorObserver = new ConcreteElevatorObserver(graphicView);
        ECElevatorConnect* simulator = new ECElevatorConnect(argv[1], elevatorObserver);
        
        // Add this line to connect the simulator
        elevatorObserver->SetSimulator(simulator);
        
        simulator->LoadSimulation();
        graphicView->Attach(elevatorObserver);
        
        // Start the simulation
        graphicView->Show();
        
        // Clean up in reverse order
        delete simulator;
        delete elevatorObserver;
        delete graphicView;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
