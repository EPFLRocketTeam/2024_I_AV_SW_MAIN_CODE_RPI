#include "Vec3.h"
#include "lv_guidance.h"
#include <cactus_rt/rt.h>
#include "guidance_interface.h"
#include <iostream>
#include "model.h"

using cactus_rt::CyclicThread;

CyclicThread::LoopControl GuidanceThread::Loop(int64_t elapsed_ns) noexcept
{
    try{
        rocket.compute_ptr(current_state, target_state,ModelPointMass::FlightMode::Landing,num_vectors_to_compute);

        // Log the computed trajectory
        std::cout << "Computed trajectory:" << std::endl;

        int state_index = 0;
        for (const auto& state : result) {
            std::cout << "State " << state_index++ << ": { ";
            for (double value : state) {
                std::cout << value << " ";
            }
            std::cout << "}" << std::endl;
        }
    }
    catch (std::exception& e){
        std::cerr << "Error in GuidanceInterface Loop: " << e.what() << '\n';
        return LoopControl::Stop;
    }

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
