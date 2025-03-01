#ifndef CYCLIC_THREAD_STATE_DEPENDANT_H
#define CYCLIC_THREAD_STATE_DEPENDANT_H

#include <cactus_rt/rt.h>
#include <map>
#include "shared_memory.h"
#include "fms_states.h"

using cactus_rt::CyclicThread;

// TODO implement:
// enum class ReturnStatus
// {
//     SUCCESS = 0,
//     FAILURE = 1
// };

class CyclicThreadStateDependant : public CyclicThread
{
    public:
        CyclicThreadStateDependant(std::string name, cactus_rt::CyclicThreadConfig config, SharedMemory<FMSState>* fms_state_memory)
            : CyclicThread(name, config), fms_state_memory(fms_state_memory) {
        }

    protected:
        LoopControl Loop(int64_t elapsed_ns) noexcept final {
            // calls the appropriate function

            // easy version without checking state is availabe
            // stateDependentFunctions[state](elapsed_ns);

            // version that checks if the state is available
            FMSState state = fms_state_memory->Read();
            auto it = stateDependentFunctions.find(state);
            if (it != stateDependentFunctions.end()) it->second();

            return LoopControl::Continue;
        }

        std::map<FMSState, std::function<void(int64_t)>> stateDependentFunctions; // defines what function should be called depending on the state

    private:
        SharedMemory<FMSState>* fms_state_memory;
}

#endif // CYCLIC_THREAD_STATE_DEPENDANT_H