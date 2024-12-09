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
    ConfigureUart();
    manager.set_module_configuration(0x02, {&control_modules.d1, &control_modules.d2, &control_modules.thrust, &control_modules.mz});
}

CyclicThread::LoopControl ComThread::Loop(int64_t elapsed_ns) noexcept
{
    ControlOutput control_data = control_memory->Read();
    std::string message;
    EncodeControlData(control_data, message);
    SendDataWishfully(message);
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

void ComThread::ConfigureUart()
{
    // Open UART device, in read-write, non-blocking mode.
    // NOCTTY means that the device is not the controlling terminal for the process.
    uart_fd = open(DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (uart_fd < 0)
    {
        throw std::runtime_error(std::string("Failed to open UART device: ") + DEVICE);
    }

    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(uart_fd, &tty) != 0)
    {
        close(uart_fd);
        throw std::runtime_error("Failed to get UART attributes.");
    }

    // Set baud rate, input/output speed
    cfsetospeed(&tty, BAUDRATE);
    cfsetispeed(&tty, BAUDRATE);

    // Configure 8N1 (8 data bits, no parity, 1 stop bit)
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit characters
    tty.c_cflag |= CLOCAL | CREAD;              // Ignore modem controls, enable reading
    tty.c_cflag &= ~(PARENB | PARODD);          // No parity
    tty.c_cflag &= ~CSTOPB;                     // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;                    // No hardware flow control

    // Raw input/output mode
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_iflag = 0;

    if (tcsetattr(uart_fd, TCSANOW, &tty) != 0)
    {
        close(uart_fd);
        throw std::runtime_error("Failed to set UART attributes.");
    }

    buffer_size = 0;
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

// TODO: This implementation by chagpt is partially incorrect.
// TODO: It is weird and does not handle partial writes correctly.
// TODO: Should be redone properly.
void ComThread::SendDataUnreliable(const byte *data, size_t data_size)
{
    // Try sending the remaining data first (if any)
    // if (buffer_size > 0)
    // {
    //     ssize_t bytes_written = write(uart_fd, buffer, buffer_size);
    //     if (bytes_written == -1)
    //     {
    //         if (errno == EAGAIN)
    //         {
    //             LOG_WARNING(Logger(), "UART is blocking. Dropped packet. Baud rate is probably not high enough for the current thread frequency.");
    //             return;
    //         }
    //         throw std::runtime_error("Failed to send data"); // Could be replaced with a warning
    //     }

    //     if (bytes_written != buffer_size)
    //     {
    //         // Could not send the entire data
    //         LOG_WARNING(Logger(), "UART still full. Dropped packet. Baud rate is probably not high enough for the current thread frequency.");
    //     }

    //     // Move the remaining data forward in the buffer
    //     buffer_size -= bytes_written;
    //     if (buffer_size > 0)
    //     {
    //         std::memmove(buffer, buffer + bytes_written, buffer_size);
    //     }
    // }

    // // If buffer is empty, try to send the new data
    // if (buffer_size == 0 && data_size > 0)
    // {
    //     ssize_t bytes_written = write(uart_fd, data, data_size);
    //     if (bytes_written == -1)
    //     {
    //         if (errno == EAGAIN)
    //         {
    //             // Could not send the entire data, store it in the buffer
    //             size_t remaining_size = data_size;
    //             if (remaining_size > buffer_size)
    //                 remaining_size = buffer_size;
    //             std::memcpy(buffer, data + bytes_written, remaining_size);
    //             buffer_size = remaining_size;
    //             return;
    //         }
    //         throw std::runtime_error("Failed to send data");
    //     }
    // }
}

// This function is temporary but works perfectly fine for high enough baud rates.
void ComThread::SendDataWishfully(const std::string &message)
{
    ssize_t bytes_written = write(uart_fd, message.c_str(), message.size());
    if (bytes_written == -1)
    {
        if (errno == EAGAIN)
            throw std::runtime_error("UART buffer is full. Baud rate is probably not high enough for the current thread frequency.");
        else
            throw std::runtime_error("Failed to send data");
    }
    if (bytes_written != static_cast<ssize_t>(message.size()))
    {
        // Could not send the entire data
        LOG_WARNING(Logger(), "Could not send the entire data! Bytes written: {}, message size: {}", bytes_written, message.size());
    }
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

void ComThread::ReceiveData()
{
    // TODO
}