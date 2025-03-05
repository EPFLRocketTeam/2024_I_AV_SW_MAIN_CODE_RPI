#ifndef CONTROL_THREAD_H
#define CONTROL_THREAD_H

#include <cactus_rt/rt.h>
#include "DroneController.h"
#include "shared_memory.h"
#include "cyclic_thread_state_dependant.h"

class ControlThread : public CyclicThreadStateDependant
{
public:
    ControlThread(  SharedMemory<FSMStates>*,
                    SharedMemory<ControlOutput>*, 
                    bool = false);

private:
    static cactus_rt::CyclicThreadConfig MakeConfig();
    Controller controller = DRONE_CONTROLLER;
    SharedMemory<ControlOutput>* control_memory;
    bool debug;

    LoopControl run(int64_t elapsed_ns) noexcept;
};

#endif // CONTROL_THREAD_H