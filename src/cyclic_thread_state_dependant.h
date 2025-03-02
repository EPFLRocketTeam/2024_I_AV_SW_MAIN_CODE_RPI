#ifndef CYCLIC_THREAD_STATE_DEPENDANT_H
#define CYCLIC_THREAD_STATE_DEPENDANT_H

#include <cactus_rt/rt.h>
#include "fsm_states.h"
#include "shared_memory.h"
#include <map>

using cactus_rt::CyclicThread;
using cactus_rt::CyclicThreadConfig;

class CyclicThreadStateDependant : public CyclicThread
{
public:
    CyclicThreadStateDependant(SharedMemory<FSMStates>* state_memory, std::string name, CyclicThreadConfig config) : state_memory(state_memory), CyclicThread(name, config) {}
    
    LoopControl Loop(int64_t elapsed_ns) noexcept final {
        FSMStates current_state = state_memory->Read();
        return stateDependentFunctions[current_state](elapsed_ns);
    }

protected:
    SharedMemory<FSMStates>* state_memory;
    std::map<FSMStates, std::function<LoopControl(int64_t)>> stateDependentFunctions; // map of what to do depending on the current state
};

#endif // CYCLIC_THREAD_STATE_DEPENDANT_H