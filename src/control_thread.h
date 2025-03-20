#ifndef CONTROL_THREAD_H
#define CONTROL_THREAD_H

#include <cactus_rt/rt.h>
#include <json.hpp>
#include "DroneController.h"
#include "shared_memory.h"

using cactus_rt::CyclicThread;
using json = nlohmann::json;

struct ControlInput
{
    State desired_state;
    State current_state;
    SetpointSelection setpointSelection;
    double inline_thrust;
};

std::ostream& operator<<(std::ostream& os, const ControlInput& input);

class ControlThread : public CyclicThread
{
public:
    ControlThread(SharedMemory<ControlInput> *control_input, SharedMemory<ControlOutput> *control_output);

protected:
    LoopControl Loop(int64_t elapsed_ns) noexcept final;

private:
    Controller ControllerFromFile(const std::string &file_path);
    Controller ControllerFromJSON(const json& doc);
    static cactus_rt::CyclicThreadConfig MakeConfig();
    std::unique_ptr<Controller> controller;
    SharedMemory<ControlInput>* control_input;
    SharedMemory<ControlOutput>* control_output;
};

#endif // CONTROL_THREAD_H