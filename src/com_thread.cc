#include "com_thread.h"
#include "shared_memory.h"
#include "DroneController.h"
#include <cactus_rt/rt.h>
#include <fcntl.h>   // File control
#include <termios.h> // Terminal I/O

constexpr int BAUDRATE = B115200;
constexpr const char *DEVICE = "/dev/serial0";

using cactus_rt::CyclicThread;

ComThread::ComThread(SharedMemory<ControlOutput> *control_memory) : CyclicThread("ComThread", MakeConfig()), control_memory(control_memory)
{
    uart_manager = new CM4UART(BAUDRATE, DEVICE, Logger());
    manager.set_module_configuration(0x02, {&control_modules.d1, &control_modules.d2, &control_modules.thrust, &control_modules.mz});
}

CyclicThread::LoopControl ComThread::Loop(int64_t elapsed_ns) noexcept
{
    ControlOutput control_data = control_memory->Read();
    std::string message;
    EncodeControlData(control_data, message);

    // TODO: Use uart_manager to transmit the message

    LOG_INFO(Logger(), "Sent encoded message: {}", message);

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

std::string ComThread::EncodeControlData(ControlOutput &control_data, std::string &message)
{
    control_modules.d1.set_value(control_data.d1);
    control_modules.d2.set_value(control_data.d2);
    control_modules.thrust.set_value(control_data.thrust);
    control_modules.mz.set_value(control_data.mz);

    const int length = 1 + 4 * 4; // 1 byte for module id, 4 bytes for each module
    char combined_buffer[length];
    std::string encoded_message;
    manager.generate_combined_message(0x02, combined_buffer, encoded_message);

    return encoded_message;
}

void ComThread::DecodeControlData(const std::string &message)
{
    char identifier;
    std::vector<std::vector<float>> values;
    manager.unpack_combined_message(message, identifier, values);

    LOG_INFO(Logger(), "Decoded values for identifier: {}", identifier);

    for (const auto &module_values : values)
    {
        std::string log_message = "Values: ";
        for (const auto &val : module_values)
        {
            log_message += std::to_string(val) + ", ";
        }
        LOG_INFO(Logger(), "{}", log_message);
    }
}