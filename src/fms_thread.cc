#include <cactus_rt/rt.h>
#include <iostream>

using cactus_rt::CyclicThread;

CyclicThread::LoopControl FMSThread::Loop(int64_t elapsed_ns) noexcept
{
    return LoopControl::Continue;
}

cactus_rt::CyclicThreadConfig FMSThread::MakeConfig()
{
    // TODO !!!
    cactus_rt::CyclicThreadConfig config;

    // Run at 200 Hz.
    config.period_ns = 80'000'000;

    // Pin this thread on CPU core #2
    config.cpu_affinity = std::vector<size_t>{3};

    // Run the thread with SCHED_FIFO at real-time priority of 80.
    config.SetFifoScheduler(70);
    return config;
}

FSMThread::INITTransition()
{
    // TODO !!!
}

FSMThread::ARMEDTransition()
{
    // TODO !!!
}

FMSThread::MANUAL_FLIGHTTransition()
{
    // TODO !!!
}

FSMThread::AUTOMATIC_FLIGHTTransition()
{
    // TODO !!!
}

FSMThread::ABORTTransition()
{
    return; // abort 
}