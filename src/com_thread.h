#ifndef COM_THREAD_H
#define COM_THREAD_H

#include <cactus_rt/rt.h>
#include "CM4UART.h"
#include "shared_memory.h"
#include "DroneController.h"
#include "god.h"

using cactus_rt::CyclicThread;

class ComThread : public CyclicThread
{
public:
    ComThread(GOD*, bool = false);

protected:
    LoopControl Loop(int64_t elapsed_ns) noexcept final;

private:
    GOD* god;
    CM4UART uart;
    bool debug;
    static cactus_rt::CyclicThreadConfig MakeConfig();
};

#endif // COM_THREAD_H