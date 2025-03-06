#include "com_thread.h"
#include "DroneController.h"
#include "control_thread.h"
#include "shared_memory.h"
#include <cactus_rt/rt.h>
#include <fcntl.h>   // File control
#include <termios.h> // Terminal I/O

constexpr int BAUDRATE = B115200;
constexpr const char *DEVICE = "/dev/serial0";

SharedMemory<ControlInput> *ComThread::control_input = nullptr;
SharedMemory<ControlOutput> *ComThread::control_output = nullptr;

using cactus_rt::CyclicThread;

ComThread::ComThread(SharedMemory<ControlInput> *control_input, SharedMemory<ControlOutput> *control_output)
    : CyclicThread("ComThread", MakeConfig())
{
    ComThread::control_input = control_input;
    ComThread::control_output = control_output;
    uart_manager = new CM4UART(BAUDRATE, DEVICE, Logger());
    ConfigureModules();
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

ComThread::~ComThread()
{
    delete uart_manager;
}

void ComThread::ConfigureModules()
{
    // Control input modules
    manager.set_module_configuration(0x02, {&control_input_modules.d1,
                                            &control_input_modules.d2,
                                            &control_input_modules.thrust,
                                            &control_input_modules.mz});

    // Control output modules
    manager.set_module_configuration(0x03, {&control_output_modules.drone_attitude,
                                            &control_output_modules.drone_rate,
                                            &control_output_modules.drone_attitude_count,
                                            &control_output_modules.drone_rate_count,
                                            &control_output_modules.remote_att_ref,
                                            &control_output_modules.remote_inline_thrust,
                                            &control_output_modules.remote_yaw_rate_ref,
                                            &control_output_modules.remote_armed});
}

CyclicThread::LoopControl ComThread::Loop(int64_t elapsed_ns) noexcept
{
    // Send control output
    ControlOutput output = control_output->Read();
    std::string sent_message = EncodeControlOutput(output);
    uart_manager->writePacket((unsigned char *)sent_message.c_str(), sent_message.size());

    // Receive control input
    unsigned char data[1024];
    size_t data_size;
    uart_manager->readPacket(data, data_size);
    std::string received_message = std::string(reinterpret_cast<char*>(data), data_size);
    DecodeControlInput(received_message);

    return LoopControl::Continue;
}

std::string ComThread::EncodeControlOutput(ControlOutput &control_output)
{
    control_input_modules.d1.set_value(control_output.d1);
    control_input_modules.d2.set_value(control_output.d2);
    control_input_modules.thrust.set_value(control_output.thrust);
    control_input_modules.mz.set_value(control_output.mz);

    const int length = 1 + 4 * 4; // 1 byte for module id, 4 bytes for each module
    char combined_buffer[length];
    std::string encoded_message;
    manager.generate_combined_message(0x02, combined_buffer, encoded_message);

    return encoded_message;
}

void ComThread::DecodeControlInput(const std::string &message)
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