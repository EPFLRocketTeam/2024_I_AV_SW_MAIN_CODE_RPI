#ifndef COM_THREAD_H
#define COM_THREAD_H

#include "CM4UART.h"
#include "shared_memory.h"
#include <cactus_rt/rt.h>
#include "god.h"

using cactus_rt::CyclicThread;

class ComThread : public CyclicThread
{
  public:
    ComThread(GOD *god);
    ~ComThread();

  protected:
    LoopControl Loop(int64_t elapsed_ns) noexcept final;

  private:
    static cactus_rt::CyclicThreadConfig MakeConfig();

    void SendDataToTeensy();
    void ReceiveDataFromTeensy();

    GOD *god;

    CM4UART *uart_manager;
};

#endif // COM_THREAD_H