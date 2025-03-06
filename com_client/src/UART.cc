#include "UART.h"
#include "Payload.h"
#include <cstring>

UART::UART()
    : readIndex(0),
      writeIndex(0),
      peekIndex(0)
{
}

void UART::RegisterHandler(int packetId, HandlerFunction handler)
{
    handlers[packetId] = handler;
}

size_t UART::AvailableBytesToPeek() const
{
    return (writeIndex - (readIndex + peekIndex) + RING_BUFFER_SIZE) % RING_BUFFER_SIZE;
}

uint8_t UART::Peek()
{
    uint8_t byte = circularBuffer[(readIndex + peekIndex) % RING_BUFFER_SIZE];
    peekIndex = (peekIndex + 1) % RING_BUFFER_SIZE;
    return byte;
}

void UART::AdvanceReadIndex(size_t amount)
{
    readIndex = (readIndex + amount) % RING_BUFFER_SIZE;
}

std::optional<uint8_t> UART::PeekUnstuff()
{
    if (AvailableBytesToPeek() < 1)
        return std::nullopt;

    uint8_t byte = Peek();

    // Handle escape sequence
    if (byte == ESCAPE_BYTE)
    {
        if (AvailableBytesToPeek() < 1)
            return std::nullopt;

        byte = Peek() ^ ESCAPE_MASK;
    }

    return byte;
}

bool UART::DiscardCurrentByteAndContinue()
{
    AdvanceReadIndex(1);
    return true;
}

// Refactored TryParsePacket method
bool UART::TryParsePacket()
{
    peekIndex = 0;

    uint8_t packetBuffer[MAX_PACKET_SIZE_UNSTUFFED];
    size_t packetBufferIndex = 0;

    // 1. Read start byte
    if (AvailableBytesToPeek() < 1)
        return false;


    uint8_t start = Peek();
    if (start != START_BYTE)
        return DiscardCurrentByteAndContinue();

    packetBuffer[packetBufferIndex++] = start;

    // 2. Read packet ID
    auto maybeId = PeekUnstuff();
    if (!maybeId.has_value())
        return false;

    uint8_t id = maybeId.value();

    // Check if ID is valid
    if (handlers.find(id) == handlers.end())
    {
        Log(LOG_LEVEL::WARNING, "Invalid packet ID received");
        return DiscardCurrentByteAndContinue();
    }

    packetBuffer[packetBufferIndex++] = id;

    // 3. Read length
    auto maybeLength = PeekUnstuff();
    if (!maybeLength.has_value())
        return false;

    uint8_t length = maybeLength.value();

    // Check if length is valid
    if (length > MAX_PAYLOAD_SIZE)
    {
        Log(LOG_LEVEL::WARNING, "Invalid packet length received");
        return DiscardCurrentByteAndContinue();
    }

    packetBuffer[packetBufferIndex++] = length;

    // 4. Read payload
    for (size_t i = 0; i < length; i++)
    {
        auto maybeByte = PeekUnstuff();
        if (!maybeByte.has_value())
            return false;
        packetBuffer[packetBufferIndex++] = maybeByte.value();
    }

    // 5. Read checksum
    auto maybeChecksum = PeekUnstuff();
    if (!maybeChecksum.has_value())
        return false;

    uint8_t checksum = maybeChecksum.value();

    // Verify checksum (excluding start byte)
    if (ComputeChecksum(packetBuffer + 1, packetBufferIndex - 1) != checksum)
    {
        Log(LOG_LEVEL::WARNING, "Invalid checksum received");
        return DiscardCurrentByteAndContinue();
    }

    packetBuffer[packetBufferIndex++] = checksum;

    // 6. Read end byte
    if (AvailableBytesToPeek() < 1)
        return false;

    uint8_t end = Peek();
    if (end != END_BYTE)
        return DiscardCurrentByteAndContinue();

    packetBuffer[packetBufferIndex++] = end;

    // 7. Process valid packet
    Payload payload(packetBuffer + 3, packetBufferIndex - 5); // Exclude start, id, length, checksum, end
    auto handler = handlers.find(id);
    handler->second(payload);

    packetsRead++;

    // Advance past this packet
    AdvanceReadIndex(peekIndex);
    return true;
}

uint8_t UART::ComputeChecksum(const uint8_t *data, size_t data_size)
{
    uint8_t checksum = 0;
    for (size_t i = 0; i < data_size; ++i)
    {
        checksum += data[i];
    }
    return checksum; // This will give the sum modulo 256, as it's a uint8_t
}

int UART::ReceiveUARTPackets()
{
    packetsRead = 0;

    // Receive new data into circular buffer
    uint8_t tempBuffer[RECEIVE_BUFFER_SIZE];
    size_t bytesReceived = Receive(tempBuffer, RECEIVE_BUFFER_SIZE);

    // Check if we might have filled the receive buffer completely
    if (bytesReceived == RECEIVE_BUFFER_SIZE)
    {
        Log(LOG_LEVEL::WARNING, "Receive buffer filled completely, might have lost data");
    }

    // Copy received data to circular buffer
    // We cannot unstuff bytes here, because it would cause errors if an ESCAPE_BYTE appears at the end of the buffer :(
    for (size_t i = 0; i < bytesReceived; i++)
    {
        circularBuffer[writeIndex] = tempBuffer[i];
        writeIndex = (writeIndex + 1) % RING_BUFFER_SIZE;
    }

    // Try to parse packets until no more can be parsed
    while (TryParsePacket())
    {
    }

    return packetsRead;
}