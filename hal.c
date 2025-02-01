#include "hal.h"

/*
STEPS:
    1. Configure GPIOs to have alternate function
    2. Configure UART
    3. In ISR do operations
*/
volatile uint8_t txBuffer[256];  // Buffer for data to be transmitted
volatile uint8_t txHead=0;      // Index of the next byte to send
volatile uint8_t txTail=0;      // Index of the byte to be sent (TXE interrupt)

volatile uint8_t receivedData = 0;
volatile bool dataReceivedFlag = false;

void spin(volatile uint32_t count) {
    while (count--) (void) 0;
}

void gpio_set_mode(uint16_t pin, uint8_t mode) {
    GPIO_TypeDef *gpio = GPIO(PINBANK(pin));
    int n = PINNO(pin);
    RCC->AHBENR |= BIT(17 + PINBANK(pin)); // Enable GPIO clock
    gpio->MODER &= ~(3U << (n * 2));
    gpio->MODER |= (mode & 3U) << (n * 2);
}

void gpioSetAF(uint16_t pin, uint8_t afNum) {
    GPIO_TypeDef *gpio = GPIO(PINBANK(pin));
    int n = PINNO(pin);

    if (n < 8) {
        gpio->AFR[0] &= ~(0xFUL << (n * 4));
        gpio->AFR[0] |= ((uint32_t)afNum << (n * 4));
    } else {
        gpio->AFR[1] &= ~(0xFUL << ((n - 8) * 4));
        gpio->AFR[1] |= ((uint32_t)afNum << ((n - 8) * 4));
    }
}

void gpio_write(uint16_t pin, bool val) {
    GPIO_TypeDef *gpio = GPIO(PINBANK(pin));
    gpio->BSRR = (1U << PINNO(pin)) << (val ? 0 : 16);
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


uint8_t uartReadByte(USART_TypeDef *uart) {
    return (uint8_t)(uart->RDR & 0xFF);
}

void uartWriteByte(USART_TypeDef *uart, uint8_t byte) {
    while ((uart->ISR & BIT(7)) == 0);
    uart->TDR = byte;
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


void USART1_IRQHandler(void){

	if(UART1->ISR & BIT(5)){
		receivedData= uartReadByte(UART1);  
		dataReceivedFlag=true;//set flag for main loop
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

