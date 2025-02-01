#ifndef UART_H
#define UART_H

#include "hal.h"

// UART buffer size
#define UART_TX_BUFFER_SIZE 256

extern volatile uint8_t receivedData;
extern volatile bool dataReceivedFlag;

void uartInit(USART_TypeDef *uart, unsigned long baud);
bool uartWriteBuf(USART_TypeDef *uart, const char *buf, size_t len);

#endif // UART_H

