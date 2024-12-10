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
        if (!result.empty()) {
         next_point=result[0];
    } else {
    // Handle the case where result is empty, if necessary
    // For example, you could throw an exception or return a default value
    throw std::runtime_error("compute_ptr returned an empty vector");
    }

        // Log the computed trajectory
        std::cout << "next point is" << std::endl;

        
            std::cout << ": { ";
            for (double value : next_point) {
                std::cout << value << ", ";
            }
            std::cout << "}" << std::endl;
        
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
