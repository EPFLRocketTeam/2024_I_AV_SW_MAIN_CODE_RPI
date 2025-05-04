#include "com_control_thread.h"
#include "priorities.h"
#include <fcntl.h>   // File control
#include <termios.h> // Terminal I/O

// Rule of thumb to compute the baudrate: baudrate =  input_payload size in bytes * 10 * frequency in Hz
// For example, for a input_payload size of 256 bytes and a frequency of 100 Hz, the minimum baudrate is 256'000
constexpr int BAUDRATE = B115200;
constexpr const char *DEVICE = "/dev/serial0";

using cactus_rt::Thread;

ComControlThread::ComControlThread() : Thread("ComControlThread", MakeConfig())
{
    uart_driver = new CM4UARTDriver(DEVICE, BAUDRATE);
    if (!uart_driver->Begin())
    {
        throw std::runtime_error("Failed to initialize UART");
    }

    try
    {
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

cactus_rt::ThreadConfig ComControlThread::MakeConfig()
{
    cactus_rt::ThreadConfig config;

    // Run at 100 Hz.
    // config.period_ns = 10'000'000;

    // Pin this thread on CPU core #3
    // config.cpu_affinity = std::vector<size_t>{3};

    config.SetFifoScheduler(static_cast<int>(RT_PRIORITY::MEDIUM));
    return config;
}

void ComControlThread::Run() noexcept
{
    while (!this->StopRequested())
    {
        try
        {
            // Wait for data to arrive on the UART device
            std::cout << "Waiting for data..." << std::endl;
            if (!uart_driver->WaitForData(1000)) // 1s timeout
            {
                continue; // Timeout
            }

            // Read the packet from the UART device
            Payload input_payload = uart_driver->ReadPacket();

            // Decode the input packet
            VehicleInputs v_input;
            v_input.deserialize(input_payload);
            if (input_payload.hasReadError())
            {
                throw std::runtime_error("Failed to read input payload");
            }

            // Run the control algorithm
            VehicleOutputs v_output = control_driver->RunControl(v_input);

            // Encode the output packet
            Payload output_payload;
            v_output.serialize(output_payload);
            if (output_payload.hasOverflow())
            {
                throw std::runtime_error("Output payload overflow");
            }

            // Send the output packet over UART
            uart_driver->WritePacketOrTimeout(1, output_payload);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Com Control Thread Error: " << e.what() << std::endl;
            continue; // Continue to the next iteration
        }
    }
}