#ifndef NAVIGATION_THREAD_H
#define NAVIGATION_THREAD_H

#include <cactus_rt/rt.h>
#include "cyclic_thread_state_dependant.h"
#include "fsm_states.h"
#include "shared_memory.h"

class NavigationThread : public CyclicThreadStateDependant
{
public:
    NavigationThread( SharedMemory<FSMStates>*,
                    SharedMemory<std::vector<double>>*, 
                    bool = false);

private:
    static cactus_rt::CyclicThreadConfig MakeConfig();
    SharedMemory<std::vector<double>>* current_state_memory; // current state of the drone (9)

    LoopControl run(int64_t) noexcept;

    bool debug;
};

#endif // NAVIGATION_THREAD_H