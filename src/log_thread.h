#ifndef LOG_THREAD_H
#define LOG_THREAD_H

#include <string>
#include <fstream>
#include <cactus_rt/rt.h>

#include "god.h"

using cactus_rt::CyclicThread;

class LogThread :  public CyclicThread
{
public:
    LogThread(GOD*);
    ~LogThread() = default;

private:
    GOD* god;
    LoopControl Loop(int64_t elapsed_ns) noexcept final;
    static cactus_rt::CyclicThreadConfig MakeConfig();
};

#endif // LOG_THREAD_H