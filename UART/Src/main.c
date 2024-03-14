#include "CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"
#include "CMSIS/Device/ST/STM32F4xx/Include/system_stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include "UART.h"
//#include "Gcode.h"
#include "steppers.h"
#include "clock_config.h"
#include "systick_delay.h"
#include "FPU.h"


int main(void)
{
	clock_config180();
	SystemCoreClockUpdate();
	enableFPU();
	SysTick_Init();

	uart3_tx_init();
	//init_stepper(5000);
	//TIM12->CCR2=(unsigned short)(50*(float)(TIM12->ARR)/100.0f);
	stepper_init(1600 ,50);
	GPIOB->ODR &= ~(1U << stepperEN);

//	GPIOB->ODR |= (1U << x_dir);
//	//volatile int motor = 0;
//	//uint32_t steps = 0;
//	oneRevolution(1,1);

	setvbuf(stdin, NULL, _IONBF, 0);

/*  char *str1 = (char *)malloc(MAX_CHARACTER_PER_LINE * sizeof(char)); // Allocate memory for input
    if (str1 == NULL) {
        printf("Memory allocation failed.");
        return 1;
    }*/
	//int motor;
    /* Loop forever */
	while(1){

/*		GPIOB->ODR |= (1U << z_dir);
		GPIOB->ODR &= ~(1U << stepperEN);
		for (int i =0; i<1600;i++){
			GPIOB->ODR |= (1U << 2);
			delay_nus(625);
			GPIOB->ODR &= ~(1U << 2);
			delay_nus(625);
		}
		GPIOB->ODR &= ~(1U << z_dir);

		for (int i =0; i<1600;i++){
			GPIOB->ODR |= (1U << 2);
			delay_nus(625);
			GPIOB->ODR &= ~(1U << 2);
			delay_nus(625);
		}*/

		//printf("Select the motor to move: ");
		//scanf(" %[^\r]d",motor);
		//printf("\n\rSelect the number of steps to move: ");
		//scanf(" %[^\r]ld",(uint32_t)steps);
		//move_axis_stepper_motor(1,1600);

/*		GPIOB->ODR |= (1U << x_dir);
		//volatile int motor = 0;
		//uint32_t steps = 0;
		move_axis_stepper_motor(1,1600);

		delay_nms(2000);
		GPIOB->ODR &= ~(1U << x_dir);

		move_axis_stepper_motor(1,1600);

		delay_nms(2000);*/
/*
//		printf("Enter G-code Command: ");
//		scanf(" %[^\r]s", str1);
//
//		//printf("\n\rEntered Gcode: %s\n\r",str1);
//		parse_gcode(str1);
*/




	}

//    free(str1);
}
