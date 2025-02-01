#include "hal.h"
#include "uart.h"

volatile uint8_t txBuffer[UART_TX_BUFFER_SIZE];
volatile uint8_t txHead = 0;
volatile uint8_t txTail = 0;

volatile uint8_t rxBuffer[UART_RX_BUFFER_SIZE];  // RX buffer for incoming data
volatile uint8_t rxHead = 0;  // Points to where the next byte will be written
volatile uint8_t rxTail = 0; 

volatile bool dataReceivedFlag = false;



// Helper function to check if there is data in the buffer
bool uartDataAvailable(void) {
	return rxHead != rxTail;
}

// Helper function to read a byte from the buffer
uint8_t uartReadByteFromBuffer(void) {
	if (rxHead != rxTail) {
		uint8_t data = rxBuffer[rxTail];
		rxTail =(uint8_t) (rxTail + 1) % UART_RX_BUFFER_SIZE;  // Circular buffer increment
		return data;
	}
	return 0;  // Return 0 if buffer is empty
}




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
	uart->CR1 |= BIT(0) | BIT(2) | BIT(3);

	//Enable RXNE interrupt
	uart->CR1 |= BIT(5);

	NVIC_EnableIRQ(USART1_IRQn);  // Enable UART1 IRQ in NVIC

}


void uartWriteBuf(USART_TypeDef *uart, char *buf, size_t len){

	for(size_t i=0;i<len;i++){
		uint8_t nextHead=(txHead+1)%sizeof(txBuffer);
		while(nextHead==txTail);//Ensures buffer does not overflow
		txBuffer[txHead]=buf[i];
		txHead=nextHead;
	}
	//Enable TXE interrupt only when new data is added
	uart->CR1 |= BIT(7);
}

//Read a single Byte from UART
static inline uint8_t uartReadByte(USART_TypeDef *uart){
	return (uint8_t)(uart->RDR & 0xFF); //Read 8 bit data from RDR reg
}

void USART1_IRQHandler(void){

	if (UART1->ISR & BIT(5)) {  // RXNE flag (Received data)
		uint8_t nextHead = (uint8_t)(rxHead + 1) % UART_RX_BUFFER_SIZE;  // Calculate the next head position
		if (nextHead != rxTail) {  // Ensure the buffer is not full
			rxBuffer[rxHead] = (uint8_t)(UART1->RDR & 0xFF);  // Read received byte
			rxHead = nextHead;  // Update head pointer
			dataReceivedFlag = true;             // Set flag to process data in main loop
		} else {
			// Handle buffer overflow (optional, you can implement overwriting or dropping data)
		}
	}	


	//Handle tx interrupt
	if(UART1->ISR & BIT(7)){
		if(txHead!=txTail){
			UART1->TDR=txBuffer[txTail];
			txTail=(txTail+1) %sizeof(txBuffer);
		}else{
			//No Data to send disable TXE interrupt
			UART1->CR1 &= ~BIT(7);
		}

	}
}


