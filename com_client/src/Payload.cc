#include "Payload.h"
#include <cstring>
#include <stdexcept>

Payload::Payload() : payloadSize(0), readPosition(0) {
    std::memset(payload, 0, MAX_SIZE);
}

Payload::Payload(const uint8_t *bytes, size_t size)
{
    if (size > MAX_SIZE)
    {
        throw std::length_error("Payload size exceeds maximum packet size");
    }

    std::memcpy(payload, bytes, size);
    payloadSize = size;
    readPosition = 0;
}

void Payload::SetBytes(const uint8_t* bytes, size_t size) {
    if (size > MAX_SIZE) {
        throw std::length_error("Payload size exceeds maximum packet size");
    }
    
    std::memcpy(payload, bytes, size);
    payloadSize = size;
    readPosition = 0;
}

const uint8_t* Payload::GetBytes() const {
    return payload;
}

size_t Payload::GetSize() const {
    return payloadSize;
}

void Payload::Write(int value) {
    if (payloadSize + sizeof(int) > MAX_SIZE) {
        throw std::length_error("Cannot write int: would exceed maximum packet size");
    }
    
    std::memcpy(payload + payloadSize, &value, sizeof(int));
    payloadSize += sizeof(int);
}

void Payload::Write(float value) {
    if (payloadSize + sizeof(float) > MAX_SIZE) {
        throw std::length_error("Cannot write float: would exceed maximum packet size");
    }
    
    std::memcpy(payload + payloadSize, &value, sizeof(float));
    payloadSize += sizeof(float);
}

void Payload::Write(bool value) {
    if (payloadSize + 1 > MAX_SIZE) {
        throw std::length_error("Cannot write bool: would exceed maximum packet size");
    }
    
    payload[payloadSize] = value ? 1 : 0;
    payloadSize += 1;
}

void Payload::Write(const uint8_t* bytes, size_t size) {
    if (payloadSize + size > MAX_SIZE) {
        throw std::length_error("Cannot write bytes: would exceed maximum packet size");
    }
    
    std::memcpy(payload + payloadSize, bytes, size);
    payloadSize += size;
}

int Payload::ReadInt() {
    if (readPosition + sizeof(int) > payloadSize) {
        throw std::out_of_range("Not enough bytes to read an int");
    }
    
    int value;
    std::memcpy(&value, payload + readPosition, sizeof(int));
    readPosition += sizeof(int);
    return value;
}

float Payload::ReadFloat() {
    if (readPosition + sizeof(float) > payloadSize) {
        throw std::out_of_range("Not enough bytes to read a float");
    }
    
    float value;
    std::memcpy(&value, payload + readPosition, sizeof(float));
    readPosition += sizeof(float);
    return value;
}

bool Payload::ReadBool() {
    if (readPosition >= payloadSize) {
        throw std::out_of_range("Not enough bytes to read a bool");
    }
    
    bool value = payload[readPosition] != 0;
    readPosition += 1;
    return value;
}

void Payload::ReadBytes(uint8_t* destBuffer, size_t length) {
    if (readPosition + length > payloadSize) {
        throw std::out_of_range("Not enough bytes to read");
    }
    
    std::memcpy(destBuffer, payload + readPosition, length);
    readPosition += length;
}

void Payload::ResetReadPosition() {
    readPosition = 0;
}

size_t Payload::GetReadPosition() const {
    return readPosition;
}

void Payload::Clear() {
    payloadSize = 0;
    readPosition = 0;
}