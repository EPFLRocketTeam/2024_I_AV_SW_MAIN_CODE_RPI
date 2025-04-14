#ifndef GOD_H
#define GOD_H

#include "shared_memory.h"
#include <iostream>
#include <vector>
#include <string>
#include "fsm_states.h"
#include "control_thread.h"

#include "shared_types/VehicleInputs.h"
#include "shared_types/VehicleOutputs.h"

struct GOD // Global Object Dictionary
{
    // FSM
    SharedMemory<FSMStates> fsm_state_memory;

    // Control
    SharedMemory<VehicleInputs> vehicle_inputs_memory; // The received data from the Teensy
    SharedMemory<VehicleOutputs> vehicle_outputs_memory; // The data to be sent to the Teensy
    SharedMemory<std::list<double>> control_state_memory; // The current state of the controller

    // Guidance
    SharedMemory<std::vector<double>> current_state_memory; // current state of the drone (9) [x, y, z, vx, vy, vz, thrust, theta, phi]
    SharedMemory<std::vector<double>> waypoint_state_memory; // desired state (waypoint) of the drone (9)
    SharedMemory<std::vector<double>> guidance_output_memory; // output of the guidance system (9)

    SharedMemory<std::vector<double>> guidance_waypoint_output_memory; // output of the guidance system (9)

    // Constructor
    GOD();
    ~GOD() = default;

    // Serialize and deserialize methods for UART communication with Teensy
    void serialize_for_Teensy(Payload &payload) const;
    void deserialize_from_Teensy(Payload &payload);

    // Log data
    std::string log_data() const;
};

#endif // GOD_H
