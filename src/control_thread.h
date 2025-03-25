#ifndef CONTROL_THREAD_H
#define CONTROL_THREAD_H

#include <cactus_rt/rt.h>
#include <json.hpp>
#include "DroneController.h"
#include "shared_memory.h"
#include "Packets.h"

using cactus_rt::CyclicThread;
using json = nlohmann::json;

struct ControlInput
{
    bool armed;                          // if the drone is armed
    State desired_state;                 // the desired state of the drone
    State current_state;                 // the current state of the drone
    SetpointSelection setpointSelection; // the setpoint selection
    double inline_thrust;                // the inline thrust
};

class ControlThread : public CyclicThread
{
public:
    ControlThread(SharedMemory<ControlInputPacket> *control_input, SharedMemory<ControlOutputPacket> *control_output);

protected:
    LoopControl Loop(int64_t elapsed_ns) noexcept final;

private:
    Controller ControllerFromFile(const std::string &file_path);
    Controller ControllerFromJSON(const json& doc);
    static cactus_rt::CyclicThreadConfig MakeConfig();
    std::unique_ptr<Controller> controller;
    SharedMemory<ControlInputPacket>* control_input;
    SharedMemory<ControlOutputPacket>* control_output;
};

#endif // CONTROL_THREAD_H