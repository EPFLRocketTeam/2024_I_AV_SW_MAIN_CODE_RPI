#ifndef CONTROL_THREAD_H
#define CONTROL_THREAD_H

#include <cactus_rt/rt.h>
#include "DroneController.h"
#include "shared_memory.h"

using cactus_rt::CyclicThread;

struct DroneState
{
    Vec3 attitude = {0, 0, 0}; // in °
    Vec3 rate = {0, 0, 0};     // in °/s
    int attitude_count = 0;
    int rate_count = 0;
};

struct AttRemoteInput
{
    Vec3 att_ref = {0, 0, 0}; // in °
    double inline_thrust;
    double yaw_rate_ref;
    bool arm = false;
};

struct ControlInput
{
    DroneState state;
    AttRemoteInput remote_input;
};

class ControlThread : public CyclicThread
{
public:
    ControlThread(SharedMemory<ControlInput> *control_input, SharedMemory<ControlOutput> *control_output);

protected:
    LoopControl Loop(int64_t elapsed_ns) noexcept final;

private:
    static cactus_rt::CyclicThreadConfig MakeConfig();
    Controller controller = DRONE_CONTROLLER;
    SharedMemory<ControlInput>* control_input;
    SharedMemory<ControlOutput>* control_output;
};

#endif // CONTROL_THREAD_H