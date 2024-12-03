#ifndef DRONE_THREAD_H
#define DRONE_THREAD_H

#include <cactus_rt/rt.h>
#include "shared_memory.h"
#include "DroneController.h"

using cactus_rt::CyclicThread;

class DriverThread : public CyclicThread
{
public:
    DriverThread(SharedMemory<ControlOutput> *control_memory);

protected:
    LoopControl Loop(int64_t elapsed_ns) noexcept final;

private:
    static cactus_rt::CyclicThreadConfig MakeConfig();
    SharedMemory<ControlOutput>* control_memory;
};

#endif // DRONE_THREAD_H