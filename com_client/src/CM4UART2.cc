#include "CM4UART2.h"
#include <fcntl.h>   // For open
#include <unistd.h>  // For read, write, close
#include <termios.h> // Terminal I/O
#include <stdexcept> // For runtime_error
#include <cstring>   // For memset
#include "quill/Quill.h" // For Logger

#include <iostream>
using namespace std;

CM4UART2::CM4UART2(): UART2()
{
    
    // this->logger = logger;

    // // Open UART device, in read-write, non-blocking mode.
    // // NOCTTY means that the device is not the controlling terminal for the process.
    // uart_fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    // if (uart_fd < 0)
    // {
    //     throw std::runtime_error(std::string("Failed to open UART device ") + device);
    // }

    // struct termios tty;
    // memset(&tty, 0, sizeof tty);

    // if (tcgetattr(uart_fd, &tty) != 0)
    // {
    //     close(uart_fd);
    //     throw std::runtime_error("Failed to get UART attributes.");
    // }

    // // Set baud rate, input/output speed
    // cfsetospeed(&tty, baudrate);
    // cfsetispeed(&tty, baudrate);

    // // Configure 8N1 (8 data bits, no parity, 1 stop bit)
    // tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit characters
    // tty.c_cflag |= CLOCAL | CREAD;              // Ignore modem controls, enable reading
    // tty.c_cflag &= ~(PARENB | PARODD);          // No parity
    // tty.c_cflag &= ~CSTOPB;                     // 1 stop bit
    // tty.c_cflag &= ~CRTSCTS;                    // No hardware flow control

    // // Raw input/output mode
    // tty.c_lflag = 0;
    // tty.c_oflag = 0;
    // tty.c_iflag = 0;

    // if (tcsetattr(uart_fd, TCSANOW, &tty) != 0)
    // {
    //     close(uart_fd);
    //     throw std::runtime_error("Failed to set UART attributes.");
    // }

    // LOG_INFO(logger, "UART set up successfully");
}

CM4UART2::~CM4UART2()
{
    close(uart_fd);
}

size_t CM4UART2::Send(const unsigned char *data, const size_t data_size)
{
    ssize_t bytes_written = write(uart_fd, data, data_size);
    if (bytes_written == -1)
    {
        // This error means that the device is busy
        if (errno == EAGAIN)
        {
            bytes_written = 0;
        }
        else
        {
            // throw std::runtime_error("Failed to send data");
            LOG_INFO(logger, "Failed to send data");
        }
    }
    return bytes_written;
}

size_t CM4UART2::Receive(unsigned char *data, const size_t data_size)
{
    ssize_t bytes_read = read(uart_fd, data, data_size);
    if (bytes_read == -1)
    {
        // This error means that the device is busy or the read was interrupted
        if (errno == EAGAIN or errno == EWOULDBLOCK or errno == EINTR)
        {
            bytes_read = 0;
        }
        else
        {
            // throw std::runtime_error("Failed to receive data");
            LOG_WARNING(logger, "Failed to receive data");
        }
    }
    return bytes_read;
}
