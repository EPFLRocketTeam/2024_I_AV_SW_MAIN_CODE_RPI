#include "control_thread.h"
#include "DroneController.h"
#include "shared_memory.h"
#include <cactus_rt/rt.h>
#include <iostream>

using cactus_rt::CyclicThread;

ControlThread::ControlThread(SharedMemory<ControlInput> *control_input, SharedMemory<ControlOutput> *control_output)
    : CyclicThread("ControlThread", MakeConfig()), control_input(control_input), control_output(control_output)
{
    controller.reset();
}

CyclicThread::LoopControl ControlThread::Loop(int64_t elapsed_ns) noexcept
{

    ControlInput input = control_input->Read();
    DroneState state = input.state;
    AttRemoteInput remote_input = input.remote_input;
    ControlOutput output = controller.attControlD(remote_input.inline_thrust, remote_input.att_ref,
                                                  remote_input.yaw_rate_ref, state.attitude, state.rate);
    control_output->Write(output);

    return LoopControl::Continue;
}

cactus_rt::CyclicThreadConfig ControlThread::MakeConfig()
{
    cactus_rt::CyclicThreadConfig config;

    // Run at 100 Hz.
    config.period_ns = 10'000'000;

    // Pin this thread on CPU core #3
    config.cpu_affinity = std::vector<size_t>{3};

    // Run the thread with SCHED_FIFO at real-time priority of 90.
    config.SetFifoScheduler(90);
    return config;
}
