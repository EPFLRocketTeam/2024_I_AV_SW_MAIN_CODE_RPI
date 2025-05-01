#include "com_control_thread.h"
#include "priorities.h"
#include <fcntl.h>   // File control
#include <termios.h> // Terminal I/O

// Rule of thumb to compute the baudrate: baudrate =  input_payload size in bytes * 10 * frequency in Hz
// For example, for a input_payload size of 256 bytes and a frequency of 100 Hz, the minimum baudrate is 256'000
constexpr int BAUDRATE = B115200;
constexpr const char *DEVICE = "/dev/serial0";

using cactus_rt::CyclicThread;

ComControlThread::ComControlThread() : CyclicThread("ComControlThread", MakeConfig())
{
    uart_driver = new CM4UARTDriver(DEVICE, BAUDRATE);
    if (!uart_driver->Begin())
    {
        throw std::runtime_error("Failed to initialize UART");
    }

    try {
        control_driver = new ControlDriver();
    }
    catch (const std::runtime_error &e)
    {
        throw std::runtime_error("Failed to initialize control driver: " + std::string(e.what()));
    }
}

ComControlThread::~ComControlThread()
{
    delete uart_driver;
    delete control_driver;
}

cactus_rt::CyclicThreadConfig ComControlThread::MakeConfig()
{
    cactus_rt::CyclicThreadConfig config;

    // Run at 100 Hz.
    config.period_ns = 10'000'000;

    // Pin this thread on CPU core #3
    // config.cpu_affinity = std::vector<size_t>{3};

    config.SetFifoScheduler(static_cast<int>(RT_PRIORITY::MEDIUM));
    return config;
}

CyclicThread::LoopControl ComControlThread::Loop(int64_t elapsed_ns) noexcept
{
    // Wait for and read a packet from UART
    Payload input_payload;
    if (!uart_driver->WaitForAndReadPacket(input_payload))
    {
        std::cerr << "Failed to wait for packet" << std::endl;
        return LoopControl::Continue;
    }

    // Decode the input packet
    VehicleInputs v_input;
    v_input.deserialize(input_payload);
    if (input_payload.hasReadError())
    {
        std::cerr << "Failed to deserialize input_payload" << std::endl;
        return LoopControl::Continue;
    }

    // Run the control algorithm
    VehicleOutputs v_output = control_driver->RunControl(v_input);

    // Encode the output packet
    Payload output_payload;
    v_output.serialize(output_payload);
    if (output_payload.hasOverflow())
    {
        std::cerr << "Failed to serialize output_payload, size exceeds limit." << std::endl;
        return LoopControl::Continue;
    }

    // Send the output packet over UART
    if (!uart_driver->WritePacketOrTimeout(1, output_payload))
    {
        std::cerr << "Failed to send packet" << std::endl;
        return LoopControl::Continue;
    }

    return LoopControl::Continue;
}