#include "hal.h"
#include "uart.h"

void SystemInit(void) {
}

int main(void) {
	uartInit(UART1, 115200);

	while (1) {
		if (dataReceivedFlag) {
			uartWriteBuf(UART1, "Received: ", 10);  // Send the "Received:" prompt

			// Send received bytes from the RX buffer
			while (uartDataAvailable()) {
				uint8_t byte = uartReadByteFromBuffer();
				uartWriteBuf(UART1, (char *)&byte, 1);  // Cast uint8_t * to char * explicitly
			}

			// Send newline and carriage return
			uartWriteBuf(UART1, (char*)"\r\n", 2);  // Using uartWriteBuf for \r\n

			dataReceivedFlag = false;  // Reset flag
		}

		// Available to do other operations
	}
}

