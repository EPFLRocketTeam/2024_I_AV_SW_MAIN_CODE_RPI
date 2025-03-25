#ifndef COM_THREAD_H
#define COM_THREAD_H

#include "CM4UART.h"
#include "Packets.h"
#include "shared_memory.h"
#include <cactus_rt/rt.h>

using cactus_rt::CyclicThread;
typedef unsigned char byte;

class ComThread : public CyclicThread
{
  public:
    ComThread(SharedMemory<ControlInputPacket> *control_input, SharedMemory<ControlOutputPacket> *control_output);
    ~ComThread();

  protected:
    LoopControl Loop(int64_t elapsed_ns) noexcept final;

  private:
    static cactus_rt::CyclicThreadConfig MakeConfig();

    bool SendControlOutput(const ControlOutputPacket &output);
    void ReceiveControlInput(Payload &payload);

    SharedMemory<ControlInputPacket> *control_input;
    SharedMemory<ControlOutputPacket> *control_output;

    CM4UART *uart_manager;
};

#endif // COM_THREAD_H