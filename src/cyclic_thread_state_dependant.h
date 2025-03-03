#ifndef CYCLIC_THREAD_STATE_DEPENDANT_H
#define CYCLIC_THREAD_STATE_DEPENDANT_H

#include <cactus_rt/rt.h>
#include "fsm_states.h"
#include "shared_memory.h"
#include <unordered_map>
#include <string>

using cactus_rt::CyclicThread;
using cactus_rt::CyclicThreadConfig;

class CyclicThreadStateDependant : public CyclicThread
{
public:
    CyclicThreadStateDependant(SharedMemory<FSMStates>* fsm_state_memory, const std::string& name, const CyclicThreadConfig& config) : fsm_state_memory(fsm_state_memory), CyclicThread(name, config) {}
    
    CyclicThread::LoopControl Loop(int64_t elapsed_ns) noexcept final {
        FSMStates current_state = fsm_state_memory->Read();
        return stateDependentFunctions[current_state](elapsed_ns);
    }

protected:
    SharedMemory<FSMStates>* fsm_state_memory;
    std::unordered_map<FSMStates, std::function<CyclicThread::LoopControl(int64_t)>> stateDependentFunctions; // map of what to do depending on the current state
};

#endif // CYCLIC_THREAD_STATE_DEPENDANT_H