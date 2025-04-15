#include "com_thread.h"
#include "control_thread.h"
#include "Packets.h"
#include "quill/Quill.h" // For Logger
#include "shared_memory.h"
#include <cactus_rt/rt.h>
#include <iostream>

#include <cstdint>

#include "god.h"
#include "fsm_states.h"

#include "fsm_thread.h"
#include "control_thread.h"
#include "guidance_thread.h"
#include "navigation_thread.h"
#include "log_thread.h"
#include "com_thread.h"
#include "fcf_module_thread.h"

using cactus_rt::App;

int main()
{
    // Create a cactus_rt app configuration
    cactus_rt::AppConfig app_config;
    app_config.logger_config.backend_thread_strict_log_timestamp_order = true;
    // app_config.logger_config.backend_thread_cpu_affinity = 1;
    app_config.logger_config.default_handlers.emplace_back(quill::file_handler("log.txt"));

    App app("RocketApp", app_config);

    // Sets up the signal handlers for SIGINT and SIGTERM
    cactus_rt::SetUpTerminationSignalHandler();

    // Global Object Dictionary
    GOD god;

    // UART Communication Thread
    std::shared_ptr<ComThread> com_thread;
    try
    {
        com_thread = app.CreateThread<ComThread>(&god.control_input, &god.vehicle_outputs_memory);
    }
    catch (std::exception &e)
    {
        std::cerr << "Failed to create ComThread: " << e.what() << std::endl;
        std::cerr << "Startup failure" << std::endl;
        return 1;
    }

    // Control Thread
    auto control_thread = app.CreateThread<ControlThread>(&god.fsm_state_memory,
                                                          &god.control_input,
                                                          &god.vehicle_outputs_memory,
                                                          &god.control_state_memory);

    // auto guidance_thread = app.CreateThread<GuidanceThread>(&god.fsm_state_memory,
    //                                                         &god.current_state_memory,
    //                                                         &god.waypoint_state_memory,
    //                                                         &god.guidance_output_memory, true);

    // auto navigation_thread = app.CreateThread<NavigationThread>(&god.fsm_state_memory,
    //                                                             &god.current_state_memory,
    //                                                             true);

    // auto fsm_thread = app.CreateThread<FSMThread>(&god.fsm_state_memory, true);

    // auto log_thread = app.CreateThread<LogThread>(&god);
    // auto fcf_thread = app.CreatThread<FCFThread>(&god.fsm_state_memory,
    //                                              &god.guidance_waypoint_output_memory,
    //                                              &god.current_state_memory, true)

    // Start the first trace session before the app starts so we capture all
    // events from the start of the app.
    app.StartTraceSession("tracing.perfetto");

    // Start the application, which starts all the registered threads (any thread
    // passed to App::RegisterThread) in the order they are registered.
    app.Start(); // NOTE: run in sudo !
    std::cout << "Started threads" << std::endl;

    // This function blocks until SIGINT or SIGTERM are received.
    cactus_rt::WaitForAndHandleTerminationSignal();

    // Stop the application
    std::cout << "Stopping threads" << std::endl;
    app.StopTraceSession();
    app.RequestStop();
    app.Join();

    std::cout << "Main thread stopped" << std::endl;
    return 0;

    TODO:
        - UART rework for one-shot and god sync
        - tracing doc in README
}
