#include "hal.h"


void SystemInit(void) {
}

int main(void) {
   
    uartInit(UART1, 115200);
    
    while (1) {
        if (dataReceivedFlag) {
            uartWriteBuf(UART1, "Received: ", 10);
            uartWriteBuf(UART1, (char *)&receivedData, 1);
            uartWriteBuf(UART1, "\r\n", 2);
            dataReceivedFlag = false;
          }  

     //Avalaible to do other operations
    }
}



