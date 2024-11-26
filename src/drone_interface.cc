#include "drone_interface.h"
#include <cactus_rt/rt.h>
#include <iostream>

using cactus_rt::CyclicThread;

DriverThread::DriverThread(SharedMemory<ControlOutput> *control_memory) : CyclicThread("DriverThread", MakeConfig()), control_memory(control_memory)
{
    
}

CyclicThread::LoopControl DriverThread::Loop(int64_t elapsed_ns) noexcept
{
    LOG_INFO(Logger(), "Shared memory: {}", control_memory->Read().thrust);

    return LoopControl::Continue;
}

cactus_rt::CyclicThreadConfig DriverThread::MakeConfig()
{
    cactus_rt::CyclicThreadConfig config;

    // Run at 100 Hz.
    config.period_ns = 10'000'000;

    // Run the thread with SCHED_FIFO at real-time priority of 80.
    config.SetFifoScheduler(90);
    return config;
}
