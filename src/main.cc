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

using cactus_rt::App;

int main()
{   
     // Create a cactus_rt app configuration
    cactus_rt::AppConfig app_config;
    app_config.logger_config.backend_thread_strict_log_timestamp_order = true;
    app_config.logger_config.backend_thread_cpu_affinity = 1;
    // app_config.logger_config.default_handlers.emplace_back(quill::file_handler("/home/ert/2024_I_AV_SW_MAIN_CODE_RPI/log.txt"));

    App app("RocketApp", app_config); 


    // Main thread logger
    auto main_logger = quill::create_logger("MainThread");
    LOG_INFO(main_logger, "Main thread started");

    // Sets up the signal handlers for SIGINT and SIGTERM
    cactus_rt::SetUpTerminationSignalHandler();

    // Global Object Dictionary
    GOD god;

    // UART Communication Thread
    std::shared_ptr<ComThread> com_thread;
    try
    {
        com_thread = app.CreateThread<ComThread>(&god.control_input, &god.control_output);
    }
    catch (std::exception &e)
    {
        LOG_ERROR(main_logger, "Failed to create ComThread: {}", e.what());
        LOG_CRITICAL(main_logger, "Startup failure");
        std::exit(1);
    }

    // Control Thread
    std::shared_ptr<ControlThread> control_thread = app.CreateThread<ControlThread>(&god.control_input, &god.control_output);

    // Start the application, which starts all the registered threads
    app.Start();
    LOG_INFO(main_logger, "Started threads");

    // Sets up the signal handlers for SIGINT and SIGTERM (by default).
    cactus_rt::SetUpTerminationSignalHandler();

    // We first create cactus_rt App object.
    
    // quill::Config cfg;
    // cfg.default_handlers.emplace_back(quill::file_handler("log.txt"));
    // quill::configure(cfg);
    // // quill::start();

    // LOG_INFO(Logger(), "This is an info log");

    std::cout << "Creating threads...\n";
    
    // auto control_thread = app.CreateThread<ControlThread>(  &god.fsm_state_memory,
    //                                                         &god.control_memory, true);

    // auto guidance_thread = app.CreateThread<GuidanceThread>(&god.fsm_state_memory,
    //                                                         &god.current_state_memory, 
    //                                                         &god.waypoint_state_memory, 
    //                                                         &god.guidance_output_memory, true);

    // auto navigation_thread = app.CreateThread<NavigationThread>(&god.fsm_state_memory,
    //                                                             &god.current_state_memory, 
    //                                                             true);

    // auto fsm_thread = app.CreateThread<FSMThread>(&god.fsm_state_memory, true);

    // auto log_thread = app.CreateThread<LogThread>(&god);    

    // Start the application, which starts all the registered threads (any thread
    // passed to App::RegisterThread) in the order they are registered.
    app.Start(); // NOTE: run in sudo !

    std::cout << "Rocket started\n";

    // This function blocks until SIGINT or SIGTERM are received.
    // cactus_rt::WaitForAndHandleTerminationSignal();
    std::this_thread::sleep_for(std::chrono::seconds(2)); // simulates waiting for the signal

    // Stop the application
    LOG_INFO(main_logger, "Stopping threads");
    std::cout << "Caught signal, requesting stop...\n";


    // We ask the application to stop, which stops all threads in the order they
    // are created. If you want the application to run indefinitely, remove this
    // line.
    app.RequestStop();
    app.Join();

    LOG_INFO(main_logger, "Main thread stopped");
    return 0;
}