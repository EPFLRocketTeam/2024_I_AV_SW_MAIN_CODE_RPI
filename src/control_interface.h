#ifndef CONTROL_INTERFACE_H
#define CONTROL_INTERFACE_H

#include <cactus_rt/rt.h>
#include "DroneController.h"

using cactus_rt::CyclicThread;

class ControlThread : public CyclicThread
{
public:
    ControlThread() : CyclicThread("ControlThread", MakeConfig()) {}

protected:
    LoopControl Loop(int64_t elapsed_ns) noexcept final;

private:
    Controller controller = DRONE_CONTROLLER;
    static cactus_rt::CyclicThreadConfig MakeConfig();
};

#endif // CONTROL_INTERFACE_H