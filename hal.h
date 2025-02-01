#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h> 
#include <stdlib.h>

#include "stm32f072xb.h"

#define FREQ 8000000  // HSI clock frequency is 8 MHz by default
#define BIT(x) (1UL << (x))
#define PIN(bank, num) ((((bank)-'A')<<8) | (num))
#define PINNO(pin) (pin & 255)
#define PINBANK(pin) (pin >> 8)
#define GPIO(bank) ((GPIO_TypeDef *) (GPIOA_BASE + 0x400U * (bank)))

enum { GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG };

#define RCC ((RCC_TypeDef *) RCC_BASE)
#define UART1 USART1

void spin(volatile uint32_t count);
void gpio_set_mode(uint16_t pin, uint8_t mode);
void gpioSetAF(uint16_t pin, uint8_t afNum);
void gpio_write(uint16_t pin, bool val);
void uartInit(USART_TypeDef *uart, unsigned long baud);
int uartReadReady(USART_TypeDef *uart);
uint8_t uartReadByte(USART_TypeDef *uart);
void uartWriteByte(USART_TypeDef *uart, uint8_t byte);
void uartWriteBuf(USART_TypeDef *uart, char *buf, size_t len);

extern volatile uint8_t receivedData;
extern volatile bool dataReceivedFlag;
extern volatile uint8_t txBuffer[256];  // Buffer for data to be transmitted
extern volatile uint8_t txHead;   // Index of the next byte to send
extern volatile uint8_t txTail;      // Index of the byte to be sent (TXE interrupt)

