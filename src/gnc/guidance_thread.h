#ifndef GUIDANCE_THREAD_H
#define GUIDANCE_THREAD_H

#include <cactus_rt/rt.h>
#include "cyclic_thread_state_dependant.h"
#include "lv_guidance.h"
#include "fsm_states.h"
#include "model.h"
#include "shared_memory.h"

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

#endif // GUIDANCE_THREAD_H