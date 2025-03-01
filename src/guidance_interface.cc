#include "Vec3.h"
#include "lv_guidance.h"
#include <cactus_rt/rt.h>
#include "guidance_interface.h"
#include <iostream>
#include "model.h"

using cactus_rt::CyclicThread;

CyclicThread::LoopControl GuidanceThread::Loop(int64_t elapsed_ns) noexcept
{
    // Read the current and target state of the drone
    std::vector current_state = current_state_memory->Read();
    std::vector target_state = waypoint_state_memory->Read();
    
    // Copmute and write guidance output
    // TODO: implement flight mode 
    std::vector guidance_output = rocket->compute(current_state, target_state, 0); // using default method, 1 vector outputed TODO
    guidance_output_memory->Write(guidance_output);
    
    return LoopControl::Continue;
}

cactus_rt::CyclicThreadConfig GuidanceThread::MakeConfig()
{
    cactus_rt::CyclicThreadConfig config;

    // Run at 200 Hz.
    config.period_ns = 80'000'000;

    // Pin this thread on CPU core #2
    config.cpu_affinity = std::vector<size_t>{3};

    // Run the thread with SCHED_FIFO at real-time priority of 80.
    config.SetFifoScheduler(70);
    return config;
}
