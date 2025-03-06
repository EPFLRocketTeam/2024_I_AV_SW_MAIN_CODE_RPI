#include "UART2.h"
#include <stdexcept>

#include <iomanip>
#include <iostream>
#include <stdio.h>

UART2::UART2()
    : currentState(State::WAITING_FOR_START),
      packetBufferIndex(0),
      expectedPayloadLength(0)
{
}

void UART2::RegisterHandler(int packet_id, HandlerFunction handler)
{
    handlers[packet_id] = handler;
}

void UART2::SendUARTPacket(const Payload &packet)
{
}

// TODO: Test and validate this method. Try every possible failure modes and edge cases.
int UART2::ReceiveUARTPackets()
{
    int packetsReceived = 0;

    // Receive data from UART
    size_t bytesReceived = Receive(receiveBuffer, RECEIVE_BUFFER_SIZE);
    receiveBufferIndex = 0;

    // Check if we might have filled the receive buffer completely
    if (bytesReceived == RECEIVE_BUFFER_SIZE)
    {
        Log(LOG_LEVEL::WARNING, "Receive buffer filled completely, might have lost data");
    }

    // Process each received byte
    // Possible improvement: Decrement the receiveBufferIndex to the byte after the start byte
    //                       when an error is detected to avoid skipping bytes.
    while (receiveBufferIndex < bytesReceived)
    {
        uint8_t byte = ReadByte();

        switch (currentState)
        {
        case State::WAITING_FOR_START:
        {
            if (byte == START_BYTE)
            {
                // Start of a new packet
                receiveBufferStartIndex = receiveBufferIndex - 1;
                packetBufferIndex = 0;
                packetBuffer[packetBufferIndex++] = byte;
                currentState = State::READING_ID;
            }
            break;
        }
        case State::READING_ID:
        {
            packetBuffer[packetBufferIndex++] = byte;

            // Check if the ID is valid
            if (handlers.find(byte) == handlers.end())
            {
                Log(LOG_LEVEL::WARNING, "Invalid packet ID received");
                receiveBufferIndex = receiveBufferStartIndex + 1; // Skip the start byte (probably noise)
                currentState = State::WAITING_FOR_START;
            }
            else
            {
                // Valid ID, go to reading length
                currentState = State::READING_LENGTH;
            }
            break;
        }
        case State::READING_LENGTH:
        {
            packetBuffer[packetBufferIndex++] = byte;
            expectedPayloadLength = byte;

            // Sanity check on packet length
            if (expectedPayloadLength > MAX_PAYLOAD_SIZE)
            {
                Log(LOG_LEVEL::WARNING, "Invalid packet length received");
                receiveBufferIndex = receiveBufferStartIndex + 1; // Skip the start byte (probably noise)
                currentState = State::WAITING_FOR_START;
            }
            else if (expectedPayloadLength == 0)
            {
                // No payload, go directly to checksum
                currentState = State::READING_CHECKSUM;
            }
            else
            {
                currentState = State::READING_PAYLOAD;
            }
            break;
        }
        case State::READING_PAYLOAD:
        {
            packetBuffer[packetBufferIndex++] = byte;

            // Check if we've read all payload bytes
            if (packetBufferIndex == expectedPayloadLength + 3) // 1 for start byte, 1 for length, 1 for ID
            {
                currentState = State::READING_CHECKSUM;
            }
            break;
        }
        case State::READING_CHECKSUM:
        {
            packetBuffer[packetBufferIndex++] = byte;

            // Verify the checksum
            uint8_t calculatedChecksum = ComputeChecksum(packetBuffer + 1, packetBufferIndex - 2); // Skip start byte and checksum

            if (calculatedChecksum == byte)
            {
                // Checksum is valid
                currentState = State::READING_END;
            }
            else
            {
                std::cout << "Calculated: 0x" << std::setw(2) << std::setfill('0') << std::hex << (int)calculatedChecksum << std::endl;
                std::cout << "Received: 0x" << std::setw(2) << std::setfill('0') << std::hex << (int)byte << std::endl;
                Log(LOG_LEVEL::WARNING, "Packet checksum verification failed");
                receiveBufferIndex = receiveBufferStartIndex + 1; // Skip the start byte (probably noise)
                currentState = State::WAITING_FOR_START;
            }
            break;
        }
        case State::READING_END:
        {
            packetBuffer[packetBufferIndex++] = byte;
            if (byte == END_BYTE)
            {
                // Extract the ID and call the handler
                int packetId = packetBuffer[1]; // Index 2 contains the ID

                // Check if there's a handler for this ID
                auto handler = handlers.find(packetId);
                if (handler != handlers.end())
                {
                    // Construct the payload and call the handler
                    Payload payload(packetBuffer + 3, packetBufferIndex - 5); // Skip start, length, ID, checksum, end
                    handler->second(payload);
                    packetsReceived++;
                }
                else
                {
                    Log(LOG_LEVEL::WARNING, "No handler found for packet ID");
                    receiveBufferIndex = receiveBufferStartIndex + 1; // Skip the start byte (probably noise)
                }
            }
            else
            {
                Log(LOG_LEVEL::WARNING, "Invalid packet end byte");
                receiveBufferIndex = receiveBufferStartIndex + 1; // Skip the start byte (probably noise)
            }

            // Reset to wait for the next packet
            currentState = State::WAITING_FOR_START;
            break;
        }
        }

        // Safety check to prevent buffer overflow
        if (packetBufferIndex >= MAX_PACKET_SIZE)
        {
            Log(LOG_LEVEL::WARNING, "Packet buffer overflow");
            currentState = State::WAITING_FOR_START;
        }
    }

    return packetsReceived;
}

uint8_t UART2::ComputeChecksum(const uint8_t *data, size_t data_size)
{
    // Print all the bytes the checksum is computed on (for debugging)
    for (size_t i = 0; i < data_size; ++i)
    {
        printf("%02X ", data[i]);
    }
    printf("\n");

    uint8_t checksum = 0;
    for (size_t i = 0; i < data_size; ++i)
    {
        checksum += data[i];
    }
    return checksum; // This will give the sum modulo 256, as it's a uint8_t
}

uint8_t UART2::ReadByte()
{
    // Read the current byte
    uint8_t byte = receiveBuffer[receiveBufferIndex++];

    // Check if it's an escape byte
    if (byte == ESCAPE_BYTE && receiveBufferIndex < RECEIVE_BUFFER_SIZE)
    {
        // It's an escape byte, read the next byte and XOR with the mask
        uint8_t nextByte = receiveBuffer[receiveBufferIndex++];
        return nextByte ^ ESCAPE_MASK;
    }

    // Return the regular byte
    return byte;
}
