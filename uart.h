#include "hal.h"

// UART buffer size
#define UART_TX_BUFFER_SIZE 256
#define UART_RX_BUFFER_SIZE  256  
#define PACKET_SIZE 5
extern volatile uint8_t receivedData;
extern volatile bool dataReceivedFlag;

extern void uartInit(USART_TypeDef *uart, unsigned long baud);
extern void uartWriteBuf(USART_TypeDef *uart, char *buf, size_t len);

extern volatile uint8_t txBuffer[UART_TX_BUFFER_SIZE];  // Buffer for data to be transmitted
extern volatile uint8_t txHead;      // Index of the next byte to send
extern volatile uint8_t txTail;      // Index of the byte to be sent (TXE interrupt)

extern volatile uint8_t rxBuffer[UART_RX_BUFFER_SIZE];
extern volatile uint8_t rxHead;
extern volatile uint8_t rxTail;

// Declare a function to retrieve data from the RX buffer
uint8_t uartReadByteFromBuffer(void);
bool uartDataAvailable(void);  // Check if there is data in the buffer
