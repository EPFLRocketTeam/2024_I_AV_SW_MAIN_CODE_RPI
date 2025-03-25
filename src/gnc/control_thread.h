#ifndef CONTROL_THREAD_H
#define CONTROL_THREAD_H

#include <cactus_rt/rt.h>
#include <json.hpp>
#include "DroneController.h"
#include "shared_memory.h"
#include "cyclic_thread_state_dependant.h"
#include "Packets.h"

using json = nlohmann::json;

class ControlThread : public CyclicThreadStateDependant
{
  public:
    ControlThread(SharedMemory<FSMStates> *,
                  SharedMemory<ControlInputPacket> *control_input,
                  SharedMemory<ControlOutputPacket> *control_output,
                  bool = false);

  private:
    Controller ControllerFromFile(const std::string &file_path);
    Controller ControllerFromJSON(const json &doc);
    static cactus_rt::CyclicThreadConfig MakeConfig();

    std::unique_ptr<Controller> controller;
    SharedMemory<ControlInputPacket>* control_input;
    SharedMemory<ControlOutputPacket>* control_output;

    bool debug;

    LoopControl run(int64_t elapsed_ns) noexcept;
};

#endif // CONTROL_THREAD_H