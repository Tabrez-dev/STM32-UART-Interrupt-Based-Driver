#include "uart.h"

volatile uint8_t txBuffer[UART_TX_BUFFER_SIZE];
volatile uint8_t txHead = 0;
volatile uint8_t txTail = 0;

volatile uint8_t receivedData = 0;
volatile bool dataReceivedFlag = false;

void uartInit(USART_TypeDef *uart, unsigned long baud) {
    uint8_t af = 1;
    uint16_t rx = 0, tx = 0;

    if (uart == UART1) RCC->APB2ENR |= BIT(14);

    if (uart == UART1) {
        tx = PIN('A', 9);
        rx = PIN('A', 10);
    }

    gpio_set_mode(tx, GPIO_MODE_AF);
    gpioSetAF(tx, af);
    gpio_set_mode(rx, GPIO_MODE_AF);
    gpioSetAF(rx, af);

    uart->CR1 = 0;
    uart->BRR = FREQ / baud;
    uart->CR1 |= BIT(0) | BIT(2) | BIT(3); // Enable UE, RE, TE

    uart->CR1 |= BIT(5);  // Enable RXNE interrupt
    NVIC_EnableIRQ(USART1_IRQn);
}


bool uartWriteBuf(USART_TypeDef *uart, const char *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        uint8_t nextHead = (txHead + 1) % UART_TX_BUFFER_SIZE;
        if (nextHead == txTail) {
            return false;  // Buffer full
        }
        txBuffer[txHead] = buf[i];
        txHead = nextHead;
    }
    uart->CR1 |= BIT(7);  // Enable TXE interrupt
    return true;  // Success
}

void USART1_IRQHandler(void) {
    if (UART1->ISR & BIT(5)) {  // RXNE flag
        receivedData = uartReadByte(UART1);
        dataReceivedFlag = true;
    }

    if (UART1->ISR & BIT(7)) {  // TXE flag
        if (txHead != txTail) {
            UART1->TDR = txBuffer[txTail];
            txTail = (txTail + 1) % UART_TX_BUFFER_SIZE;
        } else {
            UART1->CR1 &= ~BIT(7);  // Disable TXE interrupt if buffer is empty
        }
    }
}

