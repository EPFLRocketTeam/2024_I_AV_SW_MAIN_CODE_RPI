#ifndef COM_THREAD_H
#define COM_THREAD_H

#include <cactus_rt/rt.h>
#include "shared_memory.h"
#include "DroneController.h"
#include "CM4UART.h"
#include "control_thread.h"

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

    void SendControlOutput(const ControlOutput &output);
    void ReceiveControlOutput(Payload &payload);
    void ReceiveControlInput(Payload &payload);

    void WriteVec3(Vec3 vec, Payload &payload);
    Vec3 ReadVec3(Payload &payload);

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