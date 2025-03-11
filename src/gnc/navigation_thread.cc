#include <cactus_rt/rt.h>
#include <iostream>

#include "navigation_thread.h"

using cactus_rt::CyclicThread;

NavigationThread::NavigationThread( SharedMemory<FSMStates>* fsm_state_memory,
                                SharedMemory<std::vector<double>>* current_state_memory, 
                                bool debug)
: CyclicThreadStateDependant(fsm_state_memory, "NavigationThread", MakeConfig(), debug),
  current_state_memory(current_state_memory), debug(debug)
{
    // define the function to run for each state (std::bind is used to bind the method to the current instance of the class)
    stateDependentFunctions[FSMStates::AUTOMATIC_FLIGHT] = std::bind(&NavigationThread::run, this, std::placeholders::_1);

    if (debug) std::cout << "\t>NavigationThread created\n";
}

CyclicThread::LoopControl NavigationThread::run(int64_t elapsed_ns) noexcept
{
    if (debug) std::cout << "\t>NavigationThread @ " << elapsed_ns << std::endl;
    
    return LoopControl::Continue;
}

cactus_rt::CyclicThreadConfig NavigationThread::MakeConfig()
{
    cactus_rt::CyclicThreadConfig config;

    // Run at 1 Hz.
    config.period_ns = 1'000'000'000;

    // Pin this thread on CPU core #2
    config.cpu_affinity = std::vector<size_t>{3};

    // Run the thread with SCHED_FIFO at real-time priority of 80.
    config.SetFifoScheduler(70);
    return config;
}
