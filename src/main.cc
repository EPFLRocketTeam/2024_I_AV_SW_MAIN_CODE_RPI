#include <cactus_rt/rt.h>
#include <iostream>
#include "shared_memory.h"
#include "control_thread.h"
#include "com_thread.h"
#include "drone_thread.h"

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

    App app;
    auto control_thread = app.CreateThread<ControlThread>(&god.control_memory);
    auto com_thread = app.CreateThread<ComThread>(&god.control_memory);

    app.Start();
    std::cout << "Rocket started\n";

    cactus_rt::WaitForAndHandleTerminationSignal();
    std::cout << "Caught signal, requesting stop...\n";
    app.RequestStop();

    // We wait until all threads registered are done here.
    app.Join();

    std::cout << "Done\n";

    return 0;
}