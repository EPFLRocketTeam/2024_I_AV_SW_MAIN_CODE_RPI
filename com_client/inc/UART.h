// Implements a simple protocol to send packets over UART.
#ifndef UART_H
#define UART_H

#include <termios.h> // Terminal I/O
#include <cstddef>   // For size_t

constexpr int UART_BUFFER_SIZE = 1024;

class UART
{
public:
    UART(const int baudrate, const char *device);
    ~UART() = default;

    // Writes a packet to the UART device.
    // Returns true if the packet will be sent and false if it was dropped
    // (due to a baud rate too low for example).
    bool writePacket(const unsigned char *data, const size_t data_size);

    // Reads a packet from the UART device.
    // Returns true if a packet was read and false if no packet was available.
    bool readPacket(unsigned char *data, size_t &data_size);

protected:
    // These methods are specific to the UART implementation.
    // They need to be implemented by the derived classes for each platform.

    // Tries to write the data to the UART device, without blocking.
    // Returns the number of bytes written.
    virtual size_t send(const unsigned char *data, const size_t data_size) = 0;

    // Tries to read data_size bytes into *data from the UART device, without blocking.
    // Returns the number of bytes read.
    virtual size_t receive(unsigned char *data, const size_t data_size) = 0;

private:
    unsigned char transmit_buffer[UART_BUFFER_SIZE];
    size_t transmit_buffer_size = 0;

    unsigned char receive_buffer[UART_BUFFER_SIZE];
    size_t receive_buffer_size = 0;
};

#endif // UART_H