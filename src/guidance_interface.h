#ifndef GUIDANCE_INTERFACE_H
#define GUIDANCE_INTERFACE_H

#include <cactus_rt/rt.h>
// #include "cyclic_thread_state_dependant.h"
#include "lv_guidance.h"
#include "model.h"
#include "shared_memory.h"

using cactus_rt::CyclicThread;

class GuidanceThread : public CyclicThread
{
public:
    GuidanceThread( SharedMemory<std::vector<double>>*, 
                    SharedMemory<std::vector<double>>*, 
                    SharedMemory<std::vector<double>>*);

protected:
    LoopControl Loop(int64_t elapsed_ns) noexcept final;

private:
    static cactus_rt::CyclicThreadConfig MakeConfig();
    LVGuidance rocket;           // LVGuidance instance
    SharedMemory<std::vector<double>>* current_state_memory; // current state of the drone (9)
    SharedMemory<std::vector<double>>* waypoint_state_memory; // desired state (waypoint) of the drone (9)
    SharedMemory<std::vector<double>>* guidance_output_memory; // output of the guidance system (9)
};

#endif // GUIDANCE_INTERFACE_H