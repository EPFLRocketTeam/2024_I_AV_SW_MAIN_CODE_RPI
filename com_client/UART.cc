#include "UART.h"
#include <cstring> // For memcpy, memmove
#include <stdexcept>

UART::UART(const int baudrate, const char *device)
{
    transmit_buffer_size = 0;
}

bool UART::writePacket(const unsigned char *data, const size_t data_size)
{
    // Try sending the remaining previous packet first (if any)
    if (transmit_buffer_size > 0)
    {
        size_t bytes_written = transmit(transmit_buffer, transmit_buffer_size);
        transmit_buffer_size -= bytes_written;
        // This indicates a partial write, due to a full UART transmit_buffer
        if (transmit_buffer_size > 0)
        {
            std::memmove(transmit_buffer, transmit_buffer + bytes_written, transmit_buffer_size);
            return false; // The packet was dropped, because we are still waiting for the previous packet to be sent
        }
    }

    // If transmit_buffer is empty, try to send the new data
    if (data_size <= 0)
    {
        return true;
    }

    size_t bytes_written = transmit(data, data_size);
    // This indicates a partial write, due to a full UART transmit_buffer
    if (bytes_written < data_size)
    {
        size_t remaining_size = data_size - bytes_written;
        if (remaining_size > sizeof(transmit_buffer))
        {
            throw std::runtime_error("Packet too big for transmit_buffer");
        }
        std::memcpy(transmit_buffer, data + bytes_written, remaining_size);
        transmit_buffer_size = remaining_size;
    }

    return true; // The packet was either sent or buffered
}

bool UART::readPacket(unsigned char *data, size_t &data_size)
{
    // Append the new data to the receive buffer
    size_t bytes_read = receive(receive_buffer + receive_buffer_size, UART_BUFFER_SIZE - receive_buffer_size);
    receive_buffer_size += bytes_read;
    if (receive_buffer_size == UART_BUFFER_SIZE)
    {
        throw std::runtime_error("Receive buffer full. Packets are being sent too fast.");
    }

    // The index of the new line after the last byte of the packet
    int end_of_packet = -1;
    for (int i = receive_buffer_size - 1; i >= 0; --i)
    {
        if (receive_buffer[i] == '\n')
        {
            end_of_packet = i;
            break;
        }
    }

    // No packet found
    if (end_of_packet == -1)
    {
        return false; 
    }

    // The packet is empty
    if (end_of_packet == 0)
    {
        std::memmove(receive_buffer, receive_buffer + 1, receive_buffer_size - 1);
        receive_buffer_size--;
        data_size = 0;
        return true;
    }

    // The index of the first byte of the packet
    int start_of_packet = end_of_packet - 1;
    for (; start_of_packet >= 0; --start_of_packet)
    {
        if (receive_buffer[start_of_packet] == '\n')
        {
            break;
        }
    }
    start_of_packet++;

    // Copy the packet to the data buffer
    data_size = end_of_packet - start_of_packet;
    std::memcpy(data, receive_buffer + start_of_packet, data_size);
    std::memmove(receive_buffer, receive_buffer + end_of_packet + 1, receive_buffer_size - end_of_packet);
    receive_buffer_size -= end_of_packet + 1;
    return true;
}
