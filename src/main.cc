#include "com_thread.h"
#include "control_thread.h"
#include "Packets.h"
#include "quill/Quill.h" // For Logger
#include "shared_memory.h"
#include <cactus_rt/rt.h>
#include <iostream>

#include <cstdint>

using cactus_rt::App;

int main()
{
    // Cactus_rt app and logging configuration
    cactus_rt::AppConfig app_config;
    // Enable strict timestamp order - this will be slower, but logs will always appear in order
    app_config.logger_config.backend_thread_strict_log_timestamp_order = true;

    // Set the background logging thread CPU affinity
    // app_config.logger_config.backend_thread_cpu_affinity = 1;

    // Further logger configuration can be done here
    // See https://quillcpp.readthedocs.io/en/latest/quick_start.html#logging-to-file
    // for more information on how to log to a file

    // Create the cactus_rt application
    App app("MainCodeRpi", app_config);

    // Main thread logger
    auto main_logger = quill::create_logger("MainThread");
    LOG_INFO(main_logger, "Main thread started");

    // Sets up the signal handlers for SIGINT and SIGTERM
    cactus_rt::SetUpTerminationSignalHandler();

    // Global Object Dictionary
    struct GOD
    {
        SharedMemory<ControlInputPacket> control_input;
        SharedMemory<ControlOutputPacket> control_output;
    };
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

    // This function blocks until SIGINT or SIGTERM are received.
    cactus_rt::WaitForAndHandleTerminationSignal();

    // Stop the application
    LOG_INFO(main_logger, "Stopping threads");
    app.RequestStop();
    app.Join();

    LOG_INFO(main_logger, "Main thread stopped");
    return 0;
}