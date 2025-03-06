#ifndef CM4_UART_H
#define CM4_UART_H

#include "UART2.h"
#include "quill/Quill.h" // For Logger

class CM4UART2 : public UART2
{
public:
    CM4UART2();
    ~CM4UART2();

private:
    quill::Logger *logger;
    size_t Send(const unsigned char *data, const size_t data_size) override;
    size_t Receive(unsigned char *data, const size_t data_size) override;
    int uart_fd;
};

#endif // CM4_UART_H