#include "com_thread.h"
#include <fcntl.h>   // File control
#include <termios.h> // Terminal I/O

// Rule of thumb to compute the baudrate: baudrate =  payload size in bytes * 10 * frequency in Hz
// For example, for a payload size of 256 bytes and a frequency of 100 Hz, the minimum baudrate is 256'000
constexpr int BAUDRATE = B115200;
constexpr const char *DEVICE = "/dev/serial0";

using cactus_rt::CyclicThread;

ComThread::ComThread(GOD *god) : CyclicThread("ComThread", MakeConfig()), god(god)
{
    uart_manager = new CM4UART(BAUDRATE, DEVICE, Logger());

    bool success = uart_manager->Begin();
    if (!success)
    {
        throw std::runtime_error("Failed to initialize UART");
    }
}

ComThread::~ComThread()
{
    delete uart_manager;
}

cactus_rt::CyclicThreadConfig ComThread::MakeConfig()
{
    cactus_rt::CyclicThreadConfig config;

    // Run at 100 Hz.
    config.period_ns = 10'000'000;

    // Pin this thread on CPU core #3
    // config.cpu_affinity = std::vector<size_t>{3};

    config.SetFifoScheduler(static_cast<int>(RT_PRIORITY::MEDIUM));
    return config;
}

void ComThread::SendDataToTeensy()
{
    Payload payload;
    try
    {
        god->serialize_for_Teensy(payload);
    }
    catch (const std::runtime_error &e)
    {
        LOG_ERROR(Logger(), "Failed to serialize payload for Teensy: {}", e.what());
        return;
    }

    bool send_success = uart_manager->SendPacket(payload);
    if (!send_success)
    {
        LOG_ERROR(Logger(), "Failed to send packet to Teensy");
        return;
    }
}

void ComThread::ReceiveDataFromTeensy()
{
    Payload received_payload;
    bool received_success = uart_manager->ReceivePacket(received_payload);
    if (!received_success)
    {
        LOG_ERROR(Logger(), "Failed to receive packet");
        return;
    }
    if (received_payload.size() == 0)
    {
        LOG_WARNING(Logger(), "Received empty packet");
        return;
    }

    try
    {
        god->deserialize_from_Teensy(received_payload);
    }
    catch (const std::runtime_error &e)
    {
        LOG_ERROR(Logger(), "Failed to deserialize payload from Teensy: {}", e.what());
        return;
    }
}

CyclicThread::LoopControl ComThread::Loop(int64_t elapsed_ns) noexcept
{
    SendDataToTeensy();
    ReceiveDataFromTeensy();

    return LoopControl::Continue;
}