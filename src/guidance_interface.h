#pragma once

#ifndef GUIDANCE_INTERFACE_H
#define GUIDANCE_INTERFACE_H

#include <cactus_rt/rt.h>
#include "cyclic_thread_state_dependant.h"
#include "lv_guidance.h"
#include "fsm_states.h"
#include "model.h"
#include "shared_memory.h"

using cactus_rt::CyclicThread;

class GuidanceThread : public CyclicThreadStateDependant
{
public:
    GuidanceThread( SharedMemory<FSMStates>*,
                    SharedMemory<std::vector<double>>*, 
                    SharedMemory<std::vector<double>>*, 
                    SharedMemory<std::vector<double>>*,
                    bool = false);

private:
    static cactus_rt::CyclicThreadConfig MakeConfig();
    LVGuidance rocket;           // LVGuidance instance
    SharedMemory<std::vector<double>>* current_state_memory; // current state of the drone (9)
    SharedMemory<std::vector<double>>* waypoint_state_memory; // desired state (waypoint) of the drone (9)
    SharedMemory<std::vector<double>>* guidance_output_memory; // output of the guidance system (9)

    LoopControl run(int64_t) noexcept;

    bool debug;
};

#endif // GUIDANCE_INTERFACE_H