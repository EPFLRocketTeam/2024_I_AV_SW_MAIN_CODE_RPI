#ifndef FCF_MODULE_THREAD_H
#define FCF_MODULE_THREAD_H

#include <cactus_rt/rt.h>
using cactus_rt::CyclicThread;

class FCFThread : public CyclicThread
{
public:
    FCFThread() : CyclicThread("FCFThread", MakeConfig()) {}

protected:
    LoopControl Loop(int64_t elapsed_ns) noexcept final;

private:
    static cactus_rt::CyclicThreadConfig MakeConfig();
    std::vector<double> guidance;
    std::vector<double> position;
    std::vector<double> vitesse;
    //  std::vector<double> time2reach;
    std::double thrust;
    std::double pitch;
    std::double yaw;
    std::double time;
    SharedMemory<ControlOutput> *control_memory;
    std::string filename;

    bool lecture();
    void temps();
};

#endif // FCF_MODULE_THREAD_H



