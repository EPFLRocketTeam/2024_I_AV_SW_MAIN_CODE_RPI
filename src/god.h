#ifndef GOD_H
#define GOD_H

#include "shared_memory.h"
#include <iostream>
#include <vector>
#include <string>
#include "fsm_states.h"
#include "control_thread.h"

struct GOD // Global Object Dictionary
{
    // Control
    SharedMemory<ControlOutput> control_memory;

    // Guidance
    SharedMemory<std::vector<double>> current_state_memory; // current state of the drone (9)
    SharedMemory<std::vector<double>> waypoint_state_memory; // desired state (waypoint) of the drone (9)
    SharedMemory<std::vector<double>> guidance_output_memory; // output of the guidance system (9)

    SharedMemory<std::vector<double>> guidance_waypoint_output_memory; // output of the guidance system (9)

    // FSM
    SharedMemory<FSMStates> fsm_state_memory;

    // Constructor
    GOD();
    ~GOD() = default;

    // Log data
    std::string log_data() const;
};

#endif // GOD_H
