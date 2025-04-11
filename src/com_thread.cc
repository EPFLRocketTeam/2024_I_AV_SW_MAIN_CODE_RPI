#include "com_thread.h"
#include <fcntl.h>   // File control
#include <termios.h> // Terminal I/O

// using cactus_rt::CyclicThread;

constexpr int BAUDRATE = B115200;
constexpr const char *DEVICE = "/dev/serial0";

using cactus_rt::CyclicThread;

ComThread::ComThread(SharedMemory<ControlInputPacket> *control_input, SharedMemory<ControlOutputPacket> *control_output)
    : CyclicThread("ComThread", MakeConfig()), control_input(control_input), control_output(control_output)
{
    uart_manager = new CM4UART(BAUDRATE, DEVICE, Logger());
    uart_manager->RegisterHandler((int)PacketId::ControlInput, [this](Payload &payload)
                                  { ReceiveControlInput(payload); });

    bool success = uart_manager->Begin();
    if (!success)
    {
        throw std::runtime_error("Failed to initialize UART");
    }
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

CyclicThread::LoopControl ComThread::Loop(int64_t elapsed_ns) noexcept
{
    uart_manager->ReceiveUARTPackets();

    ControlOutputPacket output_packet = control_output->Read();
    SendControlOutput(output_packet);
    
    uart_manager->SendUARTPackets();

    return LoopControl::Continue;
}

bool ComThread::SendControlOutput(const ControlOutputPacket &output_packet)
{
    Payload payload;
    bool success;
    success = payload.WriteControlOutputPacket(output_packet);
    if (!success)
    {
        LOG_ERROR(Logger(), "Failed to write control output to payload");
        return false;
    }

    success = uart_manager->SendUARTPacket((int)PacketId::ControlOutput, payload);

    if (!success)
    {
        LOG_ERROR(Logger(), "Failed to send control output");
        return false;
    }

    LOG_INFO(Logger(), "Sent control output");

    return true;
}

void ComThread::ReceiveControlInput(Payload &payload)
{
    ControlInputPacket input_packet;
    bool success = payload.ReadControlInputPacket(input_packet);
    if (!success)
    {
        LOG_ERROR(Logger(), "Failed to read control input from payload");
        return;
    }

    LOG_INFO(Logger(), "Received control input");

    control_input->Write(input_packet);
}