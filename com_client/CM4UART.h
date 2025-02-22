#ifndef CM4_UART_H
#define CM4_UART_H

#include "UART.h"

class CM4UART : public UART {
public:
    CM4UART(const int baudrate, const char *device);
    ~CM4UART();

private:
    size_t transmit(const unsigned char *data, const size_t data_size) override;
    size_t receive(unsigned char *data, const size_t data_size) override;
    int uart_fd;
};

#endif // CM4_UART_H