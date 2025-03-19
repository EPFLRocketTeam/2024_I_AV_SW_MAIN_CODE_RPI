
#include "log_thread.h"

using cactus_rt::CyclicThread;

LogThread::LogThread(GOD *god)
 : CyclicThread("LogThread", MakeConfig()), god(god)
{ 

}

CyclicThread::LoopControl LogThread::Loop(int64_t elapsed_ns) noexcept
{
    std::string data = god->log_data();
    LOG_INFO(Logger(), "test");
    return LoopControl::Continue;
}

cactus_rt::CyclicThreadConfig LogThread::MakeConfig()
{
    cactus_rt::CyclicThreadConfig config;

    config.period_ns = 1'000'000'000;

    config.cpu_affinity = std::vector<size_t>{3};

    // Run the thread with SCHED_FIFO at real-time priority of 80.
    config.SetFifoScheduler(70); // 70
    return config;
}