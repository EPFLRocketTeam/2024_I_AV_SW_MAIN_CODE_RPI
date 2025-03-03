#include "Vec3.h"
#include "lv_guidance.h"
#include <cactus_rt/rt.h>
#include "guidance_interface.h"
#include <iostream>
#include "model.h"

using cactus_rt::CyclicThread;

GuidanceThread::GuidanceThread( SharedMemory<FSMStates>* fsm_state_memory,
                                SharedMemory<std::vector<double>>* current_state_memory, 
                                SharedMemory<std::vector<double>>* waypoint_state_memory, 
                                SharedMemory<std::vector<double>>* guidance_output_memory,
                                bool debug)
: CyclicThreadStateDependant(fsm_state_memory, "GuidanceThread", MakeConfig()), rocket(nullptr, new ModelPointMass(), true),
  current_state_memory(current_state_memory), waypoint_state_memory(waypoint_state_memory), guidance_output_memory(guidance_output_memory),
  debug(debug)
{
    std::cout << "Guindnace thread created\n";
    // define the function to run for each state 
    // std::bind is used to bind the method to the current instance of the class
    stateDependentFunctions[FSMStates::AUTOMATIC_FLIGHT] = std::bind(&GuidanceThread::run, this, std::placeholders::_1);

    if (debug) std::cout << "GuidanceThread created\n";
}

CyclicThread::LoopControl GuidanceThread::run(int64_t elapsed_ns) noexcept
{
    // Read the current and target state of the drone
    std::vector<double> current_state = current_state_memory->Read();
    std::vector<double> target_state = waypoint_state_memory->Read();

    // check that vectors are not empty
    if (current_state.empty() || target_state.empty()) {
        if (debug) std::cout << "GuidanceThread: current or target state is empty\n";
        return LoopControl::Continue;
    }
    
    // Compute and write guidance output
    // TODO: implement flight mode 
    rocket.compute(current_state, target_state, 1);
    // std::vector<double> guidance_output = rocket.compute(current_state, target_state)[0]; // using default method, 1 vector outputed TODO
    // guidance_output_memory->Write(guidance_output);

    if (debug) std::cout << "GuidanceThread @ " << elapsed_ns << std::endl;
    
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
