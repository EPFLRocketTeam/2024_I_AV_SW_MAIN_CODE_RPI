#ifndef COM_CONTROL_THREAD_H
#define COM_CONTROL_THREAD_H

#include <cactus_rt/rt.h>
#include "CM4Driver.h"
#include "shared_memory.h"
#include "control_driver.h"
// #include "god.h"

using cactus_rt::CyclicThread;

class ComControlThread : public CyclicThread
{
  public:
    ComControlThread();
    ~ComControlThread();

  protected:
    LoopControl Loop(int64_t elapsed_ns) noexcept final;

  private:
    static cactus_rt::CyclicThreadConfig MakeConfig();

    // GOD *god;
    CM4UARTDriver *uart_driver;
    ControlDriver *control_driver;
};

#endif // COM_CONTROL_THREAD_H