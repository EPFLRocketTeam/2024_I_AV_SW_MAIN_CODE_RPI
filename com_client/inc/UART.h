#ifndef UART_H
#define UART_H

#include "Payload.h"
#include <cstddef> // For size_t
#include <cstdint> // For uint8_t
#include <optional>
#include <unordered_map>

const uint8_t START_BYTE = 0x7E;
const uint8_t END_BYTE = 0x7F;
const uint8_t ESCAPE_BYTE = 0x7D;
const uint8_t ESCAPE_MASK = 0x20;

const size_t MAX_PAYLOAD_SIZE = 256;
const size_t MAX_PACKET_SIZE_STUFFED = (MAX_PAYLOAD_SIZE + 3) * 2 + 2;
const size_t MAX_PACKET_SIZE_UNSTUFFED = MAX_PAYLOAD_SIZE + 5;
const size_t RECEIVE_BUFFER_SIZE = 2048;
const size_t RING_BUFFER_SIZE = 1024;

// TODO: Use std::function ans std::binf instead of function pointers 
typedef void (*HandlerFunction)(Payload &);

class UART
{
  public:
    UART();
    ~UART() = default;

    // Register a packet handler function for a specific ID.
    // If the function is a method of a class, use a static method.
    // Maybe lambdas or std::function would be better
    void RegisterHandler(int packet_id, HandlerFunction handler);

    // Writes a packet to the UART device.
    // Throws an exception if the packet could not be transmitted.
    void SendUARTPacket(const Payload &packet);

    // Read bytes from the UART device and try to parse them into packets.
    // Calls the registered handler functions for each packet.
    // Returns the number of packets received.
    int ReceiveUARTPackets();

  protected:
    // These methods are specific to the UART implementation.
    // They need to be implemented by the derived classes for each platform.

    // Tries to write the data to the UART device, without blocking.
    // Returns the number of bytes written.
    virtual size_t Send(const uint8_t *data, const size_t data_size) = 0;

    // Tries to read data_size bytes into *data from the UART device, without blocking.
    // Returns the number of bytes read.
    virtual size_t Receive(uint8_t *data, const size_t data_size) = 0;

    enum class LOG_LEVEL
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
    };

    // Log a message with the specified log level.
    virtual void Log(LOG_LEVEL level, const char *message) = 0;

  private:
    uint8_t circularBuffer[RING_BUFFER_SIZE];
    size_t readIndex;  // Where we're currently reading from
    size_t writeIndex; // Where new data gets written
    size_t peekIndex;  // Where to peek next in the ring buffer

    int packetsRead; // The number of packets that have been read

    // Handlers map
    std::unordered_map<int, HandlerFunction> handlers;

    // Compute the checksum of the data.
    uint8_t ComputeChecksum(const uint8_t *data, size_t data_size);
    // Number of bytes that can be peek before the end of the ring buffer
    size_t AvailableBytesToPeek() const;
    // The raw next byte in the ring buffer. Need to check AvailableBytesToPeek() > 0 before calling!
    uint8_t Peek();
    // The unstuffed next byte in the buffer, or nullopt if we have reached the end of the buffer.
    std::optional<uint8_t> PeekUnstuff();
    // Advance the readIndex (the index of the start of the packet) by amount
    void AdvanceReadIndex(size_t amount);
    // Advance the ReadIndex by 1
    bool DiscardCurrentByteAndContinue();
    // Packet parsing method
    bool TryParsePacket();
};

#endif // UART_H