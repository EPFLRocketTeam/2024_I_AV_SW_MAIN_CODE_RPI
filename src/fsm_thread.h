#ifndef FSM_THREAD_H
#define FSM_THREAD_H

#include <cactus_rt/rt.h>
#include "shared_memory.h"
#include "cyclic_thread_state_dependant.h"

class FSMThread : public CyclicThreadStateDependant
{
public:
    FSMThread(  SharedMemory<FSMStates>*,
                bool = false);

private:
    static cactus_rt::CyclicThreadConfig MakeConfig();
    SharedMemory<FSMStates>* fsm_state_memory;
    bool debug;

    cactus_rt::CyclicThread::LoopControl run(int64_t elapsed_ns) noexcept;
};

#endif // FSM_THREAD_H