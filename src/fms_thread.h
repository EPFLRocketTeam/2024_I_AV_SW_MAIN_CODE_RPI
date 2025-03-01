#ifndef FMS_THREAD_H
#define FMS_THREAD_H

#include <cactus_rt/rt.h>
#include "shared_memory.h"
#include "fms_states.h"

using cactus_rt::CyclicThread;

class FSMThread : public CyclicThread
{
public:
    FSMThread()
        : CyclicThread("FSMThread", MakeConfig()), {
    
    }

protected:
    LoopControl Loop(int64_t elapsed_ns) noexcept final;

private:
    static cactus_rt::CyclicThreadConfig MakeConfig();
    static std::map <FMSState, std::function<void()>> state_transitions = {
        {FMSState::FMS_STATE_IDLE, &FSMThread::IDLETransition},
        {FMSState::FMS_STATE_INIT, &FSMThread::INITTransition},
        {FMSState::FMS_STATE_ARMED, &FSMThread::ARMEDTransition},
        {FMSState::FMS_STATE_MANUAL_FLIGHT, &FSMThread::MANUAL_FLIGHTTransition},
        {FMSState::FMS_STATE_AUTOMATIC_FLIGHT, &FSMThread::AUTOMATIC_FLIGHTTransition},
        {FMSState::FMS_STATE_ABORT, &FSMThread::ABORTTransition}
    };

    void IDLETransition();
    void INITTransition();
    void ARMEDTransition();
    void MANUAL_FLIGHTTransition();
    void AUTOMATIC_FLIGHTTransition();
    void ABORTTransition() { return; }

};

#endif // FMS_THREAD_H