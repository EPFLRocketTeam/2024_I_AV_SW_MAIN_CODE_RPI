#include <cactus_rt/rt.h>
#include <iostream>
#include "shared_memory.h"
#include "control_thread.h"
#include "drone_thread.h"
#include "guidance_interface.h"
#include "fsm_states.h"

using cactus_rt::App;

struct GOD // Global Object Dictionary
{
    // Control
    SharedMemory<ControlOutput> control_memory;

    // Guidance
    SharedMemory<std::vector<double>> current_state_memory; // current state of the drone (9)
    SharedMemory<std::vector<double>> waypoint_state_memory; // desired state (waypoint) of the drone (9)
    SharedMemory<std::vector<double>> guidance_output_memory; // output of the guidance system (9)

    // FMS
    SharedMemory<FSMStates> fsm_state_memory;

    // Constructor
    GOD() { 
        control_memory.Write(ControlOutput{0, 0, 0, 0}); //What should the initial values be?
        fsm_state_memory.Write(FSMStates::AUTOMATIC_FLIGHT);
        current_state_memory.Write(std::vector<double>{0, 0, 0, 0, 0, 0, 0, 0, 0});
        waypoint_state_memory.Write(std::vector<double>{0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
};


int main()
{
    GOD god; // initialise the global object dictionary

    // Sets up the signal handlers for SIGINT and SIGTERM (by default).
    cactus_rt::SetUpTerminationSignalHandler();

    // We first create cactus_rt App object.
    App app;

    std::cout << "Creating threads...\n";
    
    // auto control_thread = app.CreateThread<ControlThread>(&god.control_memory, true);
    std::cout << "\t>Control thread created\n";

    // auto driver_thread = app.CreateThread<DriverThread>(&god.control_memory);
    std::cout << "\t>Driver thread created\n";

    auto guidance_thread = app.CreateThread<GuidanceThread>(&god.fsm_state_memory,
                                                            &god.current_state_memory, 
                                                            &god.waypoint_state_memory, 
                                                            &god.guidance_output_memory, true);
    std::cout << "\t>Guidance thread created\n";

    // Start the application, which starts all the registered threads (any thread
    // passed to App::RegisterThread) in the order they are registered.
    app.Start(); // NOTE: run in sudo !

    std::cout << "Rocket started\n";

    // This function blocks until SIGINT or SIGTERM are received.
    // cactus_rt::WaitForAndHandleTerminationSignal();
    std::this_thread::sleep_for(std::chrono::seconds(1)); // simulates waiting for the signal

    std::cout << "Caught signal, requesting stop...\n";


    // We ask the application to stop, which stops all threads in the order they
    // are created. If you want the application to run indefinitely, remove this
    // line.
    app.RequestStop();

    // We wait until all threads registered are done here.
    app.Join();

    std::cout << "Done\n";

    return 0;
}