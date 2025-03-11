#include "fsm_thread.h"
#include "shared_memory.h"
#include <cactus_rt/rt.h>
#include <iostream>

using cactus_rt::CyclicThread;

FSMThread::FSMThread(   SharedMemory<FSMStates> *fsm_state_memory,
                        bool debug) 
: CyclicThreadStateDependant(fsm_state_memory, "FSMThread", MakeConfig(), debug), fsm_state_memory(fsm_state_memory), debug(debug)
{    
    // define the function to run for each state (std::bind is used to bind the method to the current instance of the class)
    stateDependentFunctions[FSMStates::IDLE] = std::bind(&FSMThread::run, this, std::placeholders::_1);

    if (debug) std::cout << "\t>FSMThread created\n";
}

CyclicThread::LoopControl FSMThread::run(int64_t elapsed_ns) noexcept
{
    if (elapsed_ns > 5'000'000'000) fsm_state_memory->Write(FSMStates::AUTOMATIC_FLIGHT); // example of state change (change to AUTOMATIC_FLIGHT after 5 seconds)

    if (debug) std::cout << "\t>FSMThread @ " << elapsed_ns << std::endl;

    return LoopControl::Continue;
}

cactus_rt::CyclicThreadConfig FSMThread::MakeConfig()
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
