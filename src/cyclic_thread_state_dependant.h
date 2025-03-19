#ifndef CYCLIC_THREAD_STATE_DEPENDANT_H
#define CYCLIC_THREAD_STATE_DEPENDANT_H

#pragma once

#include <cactus_rt/rt.h>
#include "fsm_states.h"
#include "shared_memory.h"
#include <unordered_map>
#include <string>
#include <iostream>
#include <functional>


class CyclicThreadStateDependant : public cactus_rt::CyclicThread
{
public:
    CyclicThreadStateDependant( SharedMemory<FSMStates>* fsm_state_memory, 
                                const std::string& name, const cactus_rt::CyclicThreadConfig& config, 
                                const bool debug = false) 
    : fsm_state_memory(fsm_state_memory), CyclicThread(name, config), debug(debug), name(name)
    {
        // std::cout << "CyclicThreadStateDependant created\n";
    }

    virtual ~CyclicThreadStateDependant() = default;
    
    cactus_rt::CyclicThread::LoopControl Loop(int64_t elapsed_ns) noexcept final {
        FSMStates current_state = fsm_state_memory->Read();
        // current_state = FSMStates::AUTOMATIC_FLIGHT; // TODO: remove this line
        
        if (debug) std::cout << name << " running in state " << FSMStatesToString[current_state] << ": \n";

        auto it = stateDependentFunctions.find(current_state);
        if (it != stateDependentFunctions.end()) {
            return stateDependentFunctions[current_state](elapsed_ns); // calling the function
        }
        else {
            if (debug) std::cout << "no function found" << std::endl;
            return cactus_rt::CyclicThread::LoopControl::Continue; // if the state is not found, continue
        }
    }

protected:
    SharedMemory<FSMStates>* fsm_state_memory;
    std::unordered_map<FSMStates, std::function<cactus_rt::CyclicThread::LoopControl(int64_t)>> stateDependentFunctions; // map of what to do depending on the current state
    const bool debug;
    const std::string name;
};

#endif // CYCLIC_THREAD_STATE_DEPENDANT_H