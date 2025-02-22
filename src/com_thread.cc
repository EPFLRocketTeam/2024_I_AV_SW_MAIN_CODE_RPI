#include "com_thread.h"
#include "shared_memory.h"
#include "DroneController.h"
#include <cactus_rt/rt.h>
#include <fcntl.h>   // File control
#include <termios.h> // Terminal I/O

constexpr char IDENTIFIER = 0x02;
constexpr int LENGTH = 1 + 4 * 4; // 1 byte for module id, 4 bytes for each module


using cactus_rt::CyclicThread;

ComThread::ComThread(SharedMemory<ControlOutput> *control_memory) : CyclicThread("ComThread", MakeConfig()), control_memory(control_memory)
{
    uart = new CM4UART(B9600, "/dev/serial0");
    manager.set_module_configuration(IDENTIFIER, {&control_modules.d1, &control_modules.d2, &control_modules.thrust, &control_modules.mz});
}

ComThread::~ComThread()
{
    delete uart;
}

CyclicThread::LoopControl ComThread::Loop(int64_t elapsed_ns) noexcept
{
    ControlOutput control_data = control_memory->Read();
    std::string encoded_message = EncodeControlData(control_data);

    bool success = uart->writePacket(reinterpret_cast<const unsigned char*>(encoded_message.c_str()), encoded_message.size());
    if (!success)
    {
        LOG_WARNING(Logger(), "UART buffer is full, dropped packet.");
    }

    LOG_INFO(Logger(), "Data: d1: {}, d2: {}, thrust: {}, mz: {}", control_data.d1, control_data.d2, control_data.thrust, control_data.mz);
    return LoopControl::Continue;
}

cactus_rt::CyclicThreadConfig ComThread::MakeConfig()
{
    cactus_rt::CyclicThreadConfig config;

    // Run at 100 Hz.
    config.period_ns = 10'000'0000;

    // Pin this thread on CPU core #3
    // config.cpu_affinity = std::vector<size_t>{3};

    // Run the thread with SCHED_FIFO at real-time priority of 90.
    config.SetFifoScheduler(90);
    return config;
}

std::string ComThread::EncodeControlData(ControlOutput &control_data)
{
    control_modules.d1.set_value(control_data.d1);
    control_modules.d2.set_value(control_data.d2);
    control_modules.thrust.set_value(control_data.thrust);
    control_modules.mz.set_value(control_data.mz);

    char combined_buffer[LENGTH];
    std::string encoded_message;
    manager.generate_combined_message(IDENTIFIER, combined_buffer, encoded_message);

    return encoded_message + '\n';
}



// void ComThread::DecodeControlData(const std::string &message)
// {
//     char identifier;
//     std::vector<std::vector<float>> values;
//     manager.unpack_combined_message(message, identifier, values);

//     LOG_INFO(Logger(), "Decoded values for identifier: {}", identifier);

//     for (const auto &module_values : values)
//     {
//         std::string log_message = "Values: ";
//         for (const auto &val : module_values)
//         {
//             log_message += std::to_string(val) + ", ";
//         }
//         LOG_INFO(Logger(), "{}", log_message);
//     }
// }

// void ComThread::ReceiveData()
// {
//     // TODO
// }