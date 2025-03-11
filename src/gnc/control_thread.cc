#include "control_thread.h"
#include "shared_memory.h"
#include <cactus_rt/rt.h>
#include <iostream>

using cactus_rt::CyclicThread;

ControlThread::ControlThread(   SharedMemory<FSMStates> *fsm_state_memory,
                                SharedMemory<ControlOutput> *control_memory, 
                                bool debug) 
: CyclicThreadStateDependant(fsm_state_memory, "ControlThread", MakeConfig(), debug), control_memory(control_memory), debug(debug)
{
    controller.reset();
    
    // define the function to run for each state (std::bind is used to bind the method to the current instance of the class)
    stateDependentFunctions[FSMStates::AUTOMATIC_FLIGHT] = std::bind(&ControlThread::run, this, std::placeholders::_1);

    if (debug) std::cout << "\t>ControlThread created\n";
}

CyclicThread::LoopControl ControlThread::run(int64_t elapsed_ns) noexcept
{
    ControlOutput output = controller.posControlD({0, 0, 1}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0});
    // LOG_INFO(Logger(), "Output: d1: {}, d2: {}, thrust: {}, mz: {}", output.d1, output.d2, output.thrust, output.mz);

    control_memory->Write(output);

    if (debug) std::cout << "\t>ControlThread @ " << elapsed_ns << std::endl;

    return LoopControl::Continue;
}

cactus_rt::CyclicThreadConfig ControlThread::MakeConfig()
{
    cactus_rt::CyclicThreadConfig config;

    // Run at 100 Hz.
    // config.period_ns = 10'000'000;

    // Run at 2 Hz.
    config.period_ns = 500'000'000;

    // Pin this thread on CPU core #3
    config.cpu_affinity = std::vector<size_t>{3};

    // Run the thread with SCHED_FIFO at real-time priority of 90.
    config.SetFifoScheduler(90);
    return config;
}
