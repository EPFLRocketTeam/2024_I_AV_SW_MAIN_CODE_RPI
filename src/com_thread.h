#ifndef COM_THREAD_H
#define COM_THREAD_H

#include <cactus_rt/rt.h>
#include "Client.h"
#include "shared_memory.h"
#include "DroneController.h"
#include "CM4UART.h"

using cactus_rt::CyclicThread;
typedef unsigned char byte;

class ComThread : public CyclicThread
{
public:
    ComThread(SharedMemory<ControlOutput> *control_memory);

protected:
    LoopControl Loop(int64_t elapsed_ns) noexcept final;

private:
    static cactus_rt::CyclicThreadConfig MakeConfig();
    std::string EncodeControlData(ControlOutput &control_data, std::string &message);
    void DecodeControlData(const std::string &message);

    SharedMemory<ControlOutput> *control_memory;

    CM4UART *uart_manager;

    Manager manager;
    struct ControlModules
    {
        OneFloatModule d1;
        OneFloatModule d2;
        OneFloatModule thrust;
        OneFloatModule mz;
    } control_modules;
};

#endif // COM_THREAD_H