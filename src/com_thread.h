#ifndef COM_THREAD_H
#define COM_THREAD_H

#include <cactus_rt/rt.h>
#include "Client.h"
#include "shared_memory.h"
#include "DroneController.h"

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
    void ConfigureUart();
    std::string EncodeControlData(ControlOutput &control_data, std::string &message);
    void SendDataUnreliable(const byte *data, size_t data_size);
    void SendDataWishfully(const std::string &message);
    void ReceiveData();
    void DecodeControlData(const std::string &message);

    SharedMemory<ControlOutput> *control_memory;

    Manager manager;
    struct ControlModules
    {
        OneFloatModule d1;
        OneFloatModule d2;
        OneFloatModule thrust;
        OneFloatModule mz;
    } control_modules;

    int uart_fd;
    byte buffer[1024];
    unsigned int buffer_size = 0;
};

#endif // COM_THREAD_H