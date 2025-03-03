#ifndef CYCLIC_THREAD_STATE_DEPENDANT_H
#define CYCLIC_THREAD_STATE_DEPENDANT_H

#pragma once

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
    CyclicThreadStateDependant(SharedMemory<FSMStates>* fsm_state_memory, const std::string& name, const CyclicThreadConfig& config, bool debug = false) 
    : fsm_state_memory(fsm_state_memory), CyclicThread(name, config), debug(debug)
    {
        std::cout << "CyclicThreadStateDependant created\n";
    }
    
    CyclicThread::LoopControl Loop(int64_t elapsed_ns) noexcept final {
        FSMStates current_state = fsm_state_memory->Read();
        current_state = FSMStates::AUTOMATIC_FLIGHT; // TODO: remove this line

        std::cout << "Current state: " << FSMStatesToString[current_state] << std::endl;
        auto it = stateDependentFunctions.find(current_state);
        if (it != stateDependentFunctions.end()) {
            return stateDependentFunctions[current_state](elapsed_ns); // calling the function
        }
        else {
            return CyclicThread::LoopControl::Continue; // if the state is not found, continue
        }

        return CyclicThread::LoopControl::Continue;
    }

protected:
    SharedMemory<FSMStates>* fsm_state_memory;
    std::unordered_map<FSMStates, std::function<CyclicThread::LoopControl(int64_t)>> stateDependentFunctions; // map of what to do depending on the current state
    bool debug;
};

#endif // CYCLIC_THREAD_STATE_DEPENDANT_H