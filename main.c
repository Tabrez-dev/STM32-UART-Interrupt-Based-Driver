#include "hal.h"

void SystemInit(void) {
}

int main(void) {
   
    uartInit(UART1, 115200);
    
    while (1) {
        if (dataReceivedFlag) {
        
            uartWriteBuf(UART1, "Received: ", 10);

            uartWriteByte(UART1, receivedData);  // Echo back received byte
            
            uartWriteByte(UART1, '\r');
            uartWriteByte(UART1, '\n');
            
            dataReceivedFlag = false;  // Reset flag


          }  

     //Avalaible to do other operations
    }
}



