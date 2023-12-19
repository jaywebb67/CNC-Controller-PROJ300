#include "CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"
#include <stdio.h>
#include "UART.h"




int main(void)
{
	uart3_tx_init();

	setvbuf(stdin, NULL, _IONBF, 0);

	char str1[99];

    /* Loop forever */
	while(1){
		printf("Enter G-code Command: ");
		scanf(" %[^\r]s", str1);

		printf("\n\rEntered Command: %s\n\r", str1);

	}
}
