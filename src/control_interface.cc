#include "control_interface.h"
#include "DroneController.h"
#include "Vec3.h"
#include <cactus_rt/rt.h>
#include <iostream>

using cactus_rt::CyclicThread;

CyclicThread::LoopControl ControlThread::Loop(int64_t elapsed_ns) noexcept
{
    ControlOutput output = controller.rateControlD(0, {0, 0, 0}, {0, 0, 0}, {0, 0, 0});

    LOG_INFO(Logger(), "d1: {}, d2: {}, thrust: {}, mz: {}", output.d1, output.d2, output.thrust, output.mz);

    return LoopControl::Continue;
}

cactus_rt::CyclicThreadConfig ControlThread::MakeConfig()
{
    cactus_rt::CyclicThreadConfig config;

    // Run at 100 Hz.
    config.period_ns = 10'000'000;

    // Pin this thread on CPU core #2
    config.cpu_affinity = std::vector<size_t>{3};

    // Run the thread with SCHED_FIFO at real-time priority of 80.
    config.SetFifoScheduler(90);
    return config;
}
