// #define CATCH_CONFIG_MAIN
// #include "catch.hpp"

#include "UART2.h"
#include <iostream>

// TODO: Test normal compilation then test compilation test then write tests!

int intReceived;
float floatReceived;
bool boolReceived;
uint8_t rawReceived[4];

// Handler to test receiving an integer
void intHandler(Payload &payload)
{
    intReceived = payload.ReadInt();
}

// Handler to test receiving a float
void floatHandler(Payload &payload)
{
    floatReceived = payload.ReadFloat();
}

// Handler to test receiving a bool
void boolHandler(Payload &payload)
{
    boolReceived = payload.ReadBool();
}

// Handler to test receiving raw bytes
void rawHandler(Payload &payload)
{
    payload.ReadBytes(rawReceived, 4);
}

// Fake UART class for testing
class FakeUART : public UART2
{
  public:
    FakeUART() : UART2() {}

    uint8_t send_buffer[1024];
    size_t send_buffer_size = 0;
    size_t Send(const uint8_t *data, const size_t data_size) override
    {
        if (data_size > sizeof(send_buffer))
        {
            throw std::runtime_error("Packet too big for send buffer");
        }
        std::memcpy(send_buffer, data, data_size);
        send_buffer_size = data_size;
        return data_size;
    }

    uint8_t receive_buffer[1024];
    size_t receive_buffer_size = 0;
    size_t Receive(uint8_t *data, const size_t data_size) override
    {
        if (receive_buffer_size > data_size)
        {
            throw std::runtime_error("Receive buffer too small");
        }
        std::memcpy(data, receive_buffer, receive_buffer_size);
        return receive_buffer_size;
    }

    const char *log_message;
    void Log(LOG_LEVEL level, const char *message) override
    {
        std::cout << message << std::endl;
        log_message = message;
    }
};

// TEST_CASE("Test receiving packets")
int main()
{
    FakeUART uart;

    // Test receiving a packet with an integer
    uart.RegisterHandler(1, &intHandler);
    uint8_t packet[] = {START_BYTE, 0x01, 0x04, 0x39, 0x01, 0x00, 0x00, 0x3f, END_BYTE};
    uart.receive_buffer_size = sizeof(packet);
    std::memcpy(uart.receive_buffer, packet, sizeof(packet));
    // REQUIRE(uart.ReceiveUARTPackets() == 1);
    std::cout << uart.ReceiveUARTPackets() << std::endl;
    // REQUIRE(intReceived == 313);
    std::cout << intReceived << std::endl;

    // Test receiving a packet with a float
    uart.RegisterHandler(2, &floatHandler);
    uint8_t packet2[] = {START_BYTE, 0x02, 0x04, 0xda, 0x0f, 0x49, 0x40, 0x78, END_BYTE};
    uart.receive_buffer_size = sizeof(packet2);
    std::memcpy(uart.receive_buffer, packet2, sizeof(packet2));
    // REQUIRE(uart.ReceiveUARTPackets() == 1);
    std::cout << uart.ReceiveUARTPackets() << std::endl;
    // REQUIRE(floatReceived == 3.145926f);
    std::cout << (floatReceived < 3.1415927f && floatReceived > 3.1415925) << std::endl;

    // Test receiving a packet with a bool
    uart.RegisterHandler(3, &boolHandler);
    uint8_t packet3[] = {START_BYTE, 0x03, 0x01, 0x01, 0x05, END_BYTE};
    uart.receive_buffer_size = sizeof(packet3);
    std::memcpy(uart.receive_buffer, packet3, sizeof(packet3));
    // REQUIRE(uart.ReceiveUARTPackets() == 1);
    std::cout << uart.ReceiveUARTPackets() << std::endl;
    // REQUIRE(boolReceived == false);
    std::cout << boolReceived << std::endl;

    // Test receiving a packet with raw bytes
    uart.RegisterHandler(4, &rawHandler);
    uint8_t packet4[] = {START_BYTE, 0x04, 0x04, 0x01, 0x02, 0x03, 0x04, 0x12, END_BYTE};
    uart.receive_buffer_size = sizeof(packet4);
    std::memcpy(uart.receive_buffer, packet4, sizeof(packet4));
    // REQUIRE(uart.ReceiveUARTPackets() == 1);
    std::cout << uart.ReceiveUARTPackets() << std::endl;
    // REQUIRE(rawReceived[0] == 1);
    // REQUIRE(rawReceived[1] == 2);
    // REQUIRE(rawReceived[2] == 3);
    // REQUIRE(rawReceived[3] == 4);
    std::cout << (rawReceived[0] == 1 && rawReceived[1] == 2 && rawReceived[2] == 3 && rawReceived[3] == 4) << std::endl;

    // Test noise rejection
    uint8_t packet5[] = {0xe0, ESCAPE_BYTE, 0x33, END_BYTE, 0xf6, START_BYTE, 0x21, 0x31, 0x00,
                         0x33, START_BYTE, 0x01, };
}
