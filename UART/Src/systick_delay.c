/*
 * systick_delay.c
 *
 *  Created on: Feb 28, 2024
 *      Author: jaywe
 */


#include "CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"
#include "systick_delay.h"
#include "PWR.h"

#define us_count	(SystemCoreClock/1000000)
unsigned int us_counter=0;
void SysTick_Init(void)
{

	while (SysTick_Config(us_count) != 0);
	NVIC_SetPriority(SysTick_IRQn,0);
}


void delay_nms(unsigned int ms)     //delay of n milliseconds
{
	unsigned int ms_start=us_counter;
	while((unsigned int)(us_counter-ms_start)< (ms * 1000))
	{
		Sleep();
	}
}

void delay_nus(unsigned int us)     //delay of n milliseconds
{
	unsigned int us_start=us_counter;
	while((unsigned int)(us_counter-us_start)< us)
	{
		Sleep();
	}
}

void SysTick_Handler(void)
{
	us_counter++;
}


