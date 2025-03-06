#ifndef PACKET_H
#define PACKET_H

#include <cstddef>
#include <cstdint>

class Payload {
private:
    static const size_t MAX_SIZE = 1024; // Maximum packet size in bytes
    uint8_t payload[MAX_SIZE];
    size_t payloadSize;
    size_t readPosition;

public:
    // Constructor
    Payload();
    Payload(const uint8_t* bytes, size_t size);
    
    // Payload Getters and Setters
    void SetBytes(const uint8_t* bytes, size_t size);
    const uint8_t* GetBytes() const;
    size_t GetSize() const;
    
    // Write methods for different types
    void Write(int value);
    void Write(float value);
    void Write(bool value);
    void Write(const uint8_t* bytes, size_t size);
    
    // Read methods for different types
    int ReadInt();
    float ReadFloat();
    bool ReadBool();
    void ReadBytes(uint8_t* destBuffer, size_t length);
    
    // Utility methods
    void ResetReadPosition();
    size_t GetReadPosition() const;
    void Clear();
};

#endif // PACKET_H