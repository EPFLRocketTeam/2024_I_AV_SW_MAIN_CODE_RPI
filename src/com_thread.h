#ifndef COM_THREAD_H
#define COM_THREAD_H

#include "CM4UART.h"
#include "DroneController.h"
#include "control_thread.h"
#include "shared_memory.h"
#include <cactus_rt/rt.h>

using cactus_rt::CyclicThread;
typedef unsigned char byte;

class ComThread : public CyclicThread
{
  public:
    ComThread(SharedMemory<ControlInput> *control_input, SharedMemory<ControlOutput> *control_output);
    ~ComThread();

  protected:
    LoopControl Loop(int64_t elapsed_ns) noexcept final;

  private:
    static cactus_rt::CyclicThreadConfig MakeConfig();

    bool SendControlOutput(const ControlOutput &output);
    void ReceiveControlOutput(Payload &payload);
    bool SendControlInput(const ControlInput &input);
    void ReceiveControlInput(Payload &payload);

    bool WriteVec3(Payload &payload, Vec3 vec);
    bool ReadVec3(Payload &payload, Vec3 &vec);

    enum class PacketId
    {
        ControlInput = 1,
        ControlOutput,
    };

    SharedMemory<ControlInput> *control_input;
    SharedMemory<ControlOutput> *control_output;

    CM4UART *uart_manager;
};

#endif // COM_THREAD_H