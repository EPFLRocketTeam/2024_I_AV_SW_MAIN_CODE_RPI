#ifndef CONTROL_THREAD_H
#define CONTROL_THREAD_H

#include <cactus_rt/rt.h>
#include "DroneController.h"
#include "shared_memory.h"

using cactus_rt::CyclicThread;

class ControlThread : public CyclicThread
{
public:
    ControlThread(SharedMemory<ControlOutput> *control_memory, bool = true);

protected:
    LoopControl Loop(int64_t elapsed_ns) noexcept final;

private:
    static cactus_rt::CyclicThreadConfig MakeConfig();
    Controller controller = DRONE_CONTROLLER;
    SharedMemory<ControlOutput>* control_memory;
    bool debug;
};

#endif // CONTROL_THREAD_H