#include "com_thread.h"
#include "shared_memory.h"
#include "DroneController.h"
#include <cactus_rt/rt.h>

// using cactus_rt::CyclicThread;

ComThread::ComThread(GOD *god, bool debug) 
 : CyclicThread("ComThread", MakeConfig()), god(god), uart(115200, "/dev/ttyS0", Logger()), debug(debug)
{
    uart.Begin();

    if (debug) std::cout << "ComThread created\n";
}

CyclicThread::LoopControl ComThread::Loop(int64_t elapsed_ns) noexcept
{
    unsigned char ta[] = "123";
    unsigned char* t = ta;

    // uart.Send(ta, 4);
    return LoopControl::Continue;
}

cactus_rt::CyclicThreadConfig ComThread::MakeConfig()
{
    cactus_rt::CyclicThreadConfig config;

    // Run at 100 Hz.
    config.period_ns = 10'000'000;

    // Pin this thread on CPU core #3
    // config.cpu_affinity = std::vector<size_t>{3};

    // Run the thread with SCHED_FIFO at real-time priority of 90.
    config.SetFifoScheduler(90);
    return config;
}