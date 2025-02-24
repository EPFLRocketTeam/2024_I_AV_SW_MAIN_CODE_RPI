#include <cactus_rt/rt.h>
#include <iostream>
#include "shared_memory.h"
#include "control_thread.h"
#include "com_thread.h"

using cactus_rt::App;

int main()
{
    struct GOD // Global Object Dictionary
    {
        SharedMemory<ControlOutput> control_memory;
    };
    GOD god;
    god.control_memory.Write(ControlOutput{0, 0, 0, 0}); 

    // Sets up the signal handlers for SIGINT and SIGTERM (by default).
    cactus_rt::SetUpTerminationSignalHandler();

    // We first create cactus_rt App object.
    App app;

    // auto control_thread = app.CreateThread<ControlThread>(&god.control_memory);
    auto com_thread = app.CreateThread<ComThread>(&god.control_memory);

    // Start the application, which starts all the registered threads (any thread
    // passed to App::RegisterThread) in the order they are registered.
    app.Start();

    std::cout << "Rocket started\n";

    // This function blocks until SIGINT or SIGTERM are received.
    cactus_rt::WaitForAndHandleTerminationSignal();

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