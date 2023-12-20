#include "CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include "UART.h"
#include "Gcode.h"



int main(void)
{
	uart3_tx_init();

	setvbuf(stdin, NULL, _IONBF, 0);

    char *str1 = (char *)malloc(MAX_CHARACTER_PER_LINE * sizeof(char)); // Allocate memory for input
    if (str1 == NULL) {
        printf("Memory allocation failed. Exiting...");
        return 1;
    }

    /* Loop forever */
	while(1){
		printf("Enter G-code Command: ");
		scanf(" %[^\r]s", str1);

		//printf("\n\rEntered Gcode: %s\n\r",str1);
		parse_gcode(str1);


	}

    free(str1);
}
