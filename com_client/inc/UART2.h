#ifndef UART_H_2
#define UART_H_2

#include "Payload.h"
#include "quill/Quill.h" // For Logger
#include <cstddef>       // For size_t
#include <cstdint>       // For uint8_t
#include <unordered_map>

const uint8_t START_BYTE = 0x7E;
const uint8_t END_BYTE = 0x7F;
const uint8_t ESCAPE_BYTE = 0x7D;
const uint8_t ESCAPE_MASK = 0x20;
const size_t MAX_PAYLOAD_SIZE = 256;
const size_t MAX_PACKET_SIZE = (MAX_PAYLOAD_SIZE + 3) * 2;
const size_t RECEIVE_BUFFER_SIZE = 2048;

typedef void (*HandlerFunction)(Payload &);

class UART2
{
  public:
    UART2();
    ~UART2() = default;

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
    // Packet parsing state
    enum class State
    {
        WAITING_FOR_START,
        READING_LENGTH,
        READING_ID,
        READING_PAYLOAD,
        READING_CHECKSUM,
        READING_END,
    };

    State currentState;                         // Current state of the packet parser
    uint8_t receiveBuffer[RECEIVE_BUFFER_SIZE]; // Buffer for the received data
    size_t receiveBufferIndex;                  // Index of the next byte to read in the buffer
    size_t receiveBufferStartIndex;             // Index of the start byte of the current packet in the buffer
    uint8_t packetBuffer[MAX_PACKET_SIZE];      // Buffer for the current packet
    size_t packetBufferIndex;                   // Index of the next byte to write in the buffer
    size_t expectedPayloadLength;               // Expected length of the payload

    // Handlers map
    std::unordered_map<int, HandlerFunction> handlers;

    // Compute the checksum of the data.
    uint8_t ComputeChecksum(const uint8_t *data, size_t data_size);

    // Read the next byte from the UART stream, unstuffing if necessary.
    uint8_t ReadByte();
};

#endif // UART_H_2