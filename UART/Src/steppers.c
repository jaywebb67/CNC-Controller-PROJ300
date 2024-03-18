/*
 * steppers.c
 *
 *  Created on: Feb 21, 2024
 *      Author: jaywe
 */

#include "steppers.h"
#include "UART.h"
#include <stdio.h>
#include "CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"


volatile uint32_t zStepElapsed = 0;

volatile uint32_t yStepElapsed = 0;

volatile uint32_t xStepElapsed = 0;

volatile uint32_t xDesiredSteps = 0;

volatile uint32_t yDesiredSteps = 0;

volatile uint32_t zDesiredSteps = 0;

volatile bool countSteps;

void init_stepper(unsigned int F_pwm){


	#define AF_tim3	2u
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOBEN;
	GPIOB->MODER&=~(GPIO_MODER_MODER15|GPIO_MODER_MODER9|GPIO_MODER_MODER4);
	GPIOB->MODER|=GPIO_MODER_MODER15_1 |GPIO_MODER_MODER15_0 | GPIO_MODER_MODER9_0;

	GPIOB->OTYPER &= ~((1U << stepperEN)|(1U << x_dir));
	GPIOB->OTYPER |= (1U << stepperEN) | (1U << x_dir);

	GPIOB->PUPDR &= ~((3U<<(2*stepperEN))|(3U<<(2*x_dir)));
	GPIOB->ODR |= (1U << stepperEN)|(1U << x_dir);

	GPIOB->AFR[0]&=~GPIO_AFRL_AFSEL0;
	GPIOB->AFR[0]|=(AF_tim3<<GPIO_AFRL_AFSEL0_Pos);

	RCC->APB1ENR|=RCC_APB1ENR_TIM3EN;
	unsigned short psc3=6;
	unsigned short arr3=(unsigned short)(((APB1_TIM_FREQ/(psc3*F_pwm)))-1);
	TIM12->CCMR1=(6u<<TIM_CCMR1_OC2M_Pos);
	TIM12->CCER|=TIM_CCER_CC2E;
	TIM12->PSC=psc3-1;
	TIM12->ARR=arr3;
	TIM12->CNT=0;
	TIM12->CR1|=TIM_CR1_CEN;

}


void stepper_init(uint32_t frequency, uint8_t d){

	countSteps = 0;
	/*1.Enable clock access to GPIOb*/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	//clear pins corresponding to motor signals
	GPIOB->MODER &= ~((3U<<(2*2))|(3U<<(2*14))|(3U<<(2*y_dir)) |(3U<<(2*x_dir)) |(3U<<(2*z_step)) |(3U<<(2*z_dir)) |(3U<<(2*y_step)) |(3U<<(2*x_step))|(3U<<(2*stepperEN)));


	//set motor dir pins and stepperEN to output
	GPIOB->MODER |=	 (1U<<(2*2))|(1U<<(1*14))|(1U<<(1*y_dir))|(1U<<(2*x_dir))|(1U<<(2*z_dir))|(1U<<(2*stepperEN));

	//set motor step pins to AF
	GPIOB->MODER |=	 (2U<<(1*z_step))|(2U<<(2*y_step))|(1U<<(2*x_step));

	GPIOB->OTYPER &= ~(1U << stepperEN);
	GPIOB->OTYPER |= (1U << stepperEN);

/*
	//clear pin output speed
	GPIOB->OSPEEDR &= ~((3U<<(2*y_dir)) |(3U<<(2*x_dir)) |(3U<<(2*z_step)) |(3U<<(2*z_dir)) |(3U<<(2*y_step)) |(3U<<(2*x_step))|(3U<<(2*stepperEN)));

	//set pins to high speed
	GPIOB->OSPEEDR |= (3U<<(2*y_dir)) |(3U<<(2*x_dir)) |(3U<<(2*z_step)) |(3U<<(2*z_dir)) |(3U<<(2*y_step)) |(3U<<(2*x_step))|(3U<<(2*stepperEN));
*/

	//no internal pull up
	GPIOB->PUPDR &= ~((3U<<(2*y_dir)) |(3U<<(2*x_dir)) |(3U<<(2*z_step)) |(3U<<(2*z_dir)) |(3U<<(2*y_step)) |(3U<<(2*x_step))|(3U<<(2*stepperEN)));

	GPIOB->ODR |= (1U << stepperEN);

	// set step pins to timer functions - pin 5 located in AF low reg,
	GPIOB->AFR[0] &= ~(15U<<(4*z_step));
	// TIM3 channel 2 = AF2
	GPIOB->AFR[0] |= (2U<<(4*z_step));
	//pin 8 & 15 located in AF high register
	GPIOB->AFR[1] &= ~((15U<<(4*(y_step-8)))|(15U<<(4*(x_step-8))));
	// TIM4 channel 3 = AF2, TIM12 channel 2 = AF9
	GPIOB->AFR[1] |= (2U<<(4*(y_step-8)))|(9U<<(4*(x_step-8)));


	//init timers for the step signals of each axis
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN|RCC_APB1ENR_TIM4EN|RCC_APB1ENR_TIM12EN;

	//Set timer channels to outputs
	TIM3->CCMR1 &= ~TIM_CCMR1_CC2S;
	TIM4->CCMR2 &= ~TIM_CCMR2_CC3S;
	TIM12->CCMR1 &= ~TIM_CCMR1_CC2S;

	//set prescalers to 0
	TIM3->PSC = 6-1;
	TIM4->PSC = 6-1;
	TIM12->PSC = 6-1;

	// APB1 Timer clock/3600 = 25kHz
	TIM3->ARR = (APB1_TIM_FREQ / frequency) - 1;
	TIM4->ARR = (APB1_TIM_FREQ / frequency) - 1;
	TIM12->ARR = (APB1_TIM_FREQ / frequency) - 1;

	//set duty cycle
	TIM3->CCR2=(TIM3->ARR +1)/2;
	TIM4->CCR3=((TIM4->ARR +1)/2);
	TIM12->CCR2=((TIM12->ARR +1)/2);

	//Set timer to PWM mode 1
	TIM3->CCMR1 |= (6U << TIM_CCMR1_OC2M_Pos);
	TIM4->CCMR2 |= (6U << TIM_CCMR2_OC3M_Pos);
	TIM12->CCMR1 |= (6U << TIM_CCMR1_OC2M_Pos);

	//Set preload bit in CCMRx register
	TIM3->CCMR1 |= TIM_CCMR1_OC2PE;
	TIM4->CCMR2 |= TIM_CCMR2_OC3PE;
	TIM12->CCMR1 |= TIM_CCMR1_OC2PE;

	//Set Auto-reload preload enable bit in CR1 register
	TIM3->CR1 |= TIM_CR1_ARPE;
	TIM4->CR1 |= TIM_CR1_ARPE;
	TIM12->CR1 |= TIM_CR1_ARPE;

	//Select polarity - active high
	TIM3->CCER &= TIM_CCER_CC2P;
	TIM4->CCER &= ~TIM_CCER_CC3P;
	TIM12->CCER &= ~TIM_CCER_CC2P;

	//Enable timer channels
	TIM3->CCER |= TIM_CCER_CC2E;
	TIM4->CCER |= TIM_CCER_CC3E;
	TIM12->CCER |= TIM_CCER_CC2E;

	TIM3->CNT = 0;
	TIM4->CNT = 0;
	TIM12->CNT = 0;

	TIM3->CR1 |= TIM_CR1_URS;
	TIM4->CR1 |= TIM_CR1_URS;
	TIM12->CR1 |= TIM_CR1_URS;

	TIM3->EGR |= TIM_EGR_UG;
	TIM4->EGR |= TIM_EGR_UG;
	TIM12->EGR |= TIM_EGR_UG;

	// Enable timer interrupt generation
	TIM3->DIER |= TIM_DIER_UIE;
	TIM4->DIER |= TIM_DIER_UIE;
    TIM12->DIER |= TIM_DIER_UIE;

    TIM3->SR &= ~(TIM_SR_CC2IF|TIM_SR_UIF);
    TIM4->SR &= ~(TIM_SR_CC3IF|TIM_SR_UIF);
    TIM12->SR &= ~(TIM_SR_CC2IF|TIM_SR_UIF);

	// Enable TIM3 interrupt
	NVIC_SetPriority(TIM3_IRQn, 2);
	NVIC_EnableIRQ(TIM3_IRQn);

	// Enable TIM4 interrupt
	NVIC_SetPriority(TIM4_IRQn, 1);
	NVIC_EnableIRQ(TIM4_IRQn);

	// Enable TIM12 interrupt
	NVIC_SetPriority(TIM8_BRK_TIM12_IRQn, 0);
	NVIC_EnableIRQ(TIM8_BRK_TIM12_IRQn);

	//enable clocks
	TIM3->CR1 |= TIM_CR1_CEN;
	TIM4->CR1 |= TIM_CR1_CEN;
	TIM12->CR1 |= TIM_CR1_CEN;

}


void set_PWMdutycycle(uint8_t timr, uint32_t d){

	if(d > 100){
		d = 100;
	}
	else if(d < 0){
		d = 0;
	}

	switch(timr){
		case 3:
			TIM3->CCR2=(unsigned short)(d*(float)(TIM3->ARR)/100.0f);
			TIM3->EGR |= (1<<0);
			break;
		case 4:
			TIM4->CCR3=(unsigned short)(d*(float)(TIM4->ARR)/100.0f);
			TIM4->EGR |= (1<<0);
			break;
		case 12:
			TIM12->CCR2=(unsigned short)(d*(float)(TIM12->ARR)/100.0f);
			TIM12->EGR |= (1<<0);
			break;
		default:
			break;
	}
}

void setPWMfrequency(uint8_t timr, uint32_t frequency){

	if (frequency > 500000){
		frequency = 500000;
	}
	else if(frequency < 0){
		frequency = 1;
	}
	switch(timr){
		case 3:
			TIM3->ARR = (APB1_TIM_FREQ / frequency) - 1;
			TIM3->CCR2=(unsigned short)(50*(float)(TIM3->ARR)/100.0f);
			TIM3->EGR |= (1<<0);
			break;
		case 4:
			TIM4->ARR = (APB1_TIM_FREQ / frequency) - 1;
			TIM4->CCR3=(unsigned short)(50*(float)(TIM4->ARR)/100.0f);
			TIM4->EGR |= (1<<0);
			break;
		case 12:
			TIM12->ARR = (APB1_TIM_FREQ / frequency) - 1;
			TIM12->CCR2=(unsigned short)(50*(float)(TIM12->ARR)/100.0f);
			TIM12->EGR |= (1<<0);
			break;
		default:
			break;
	}
}

void move_axis_stepper_motor(int axis, uint32_t steps){

//	axis = toupper(axis);
	switch(axis){
		case 1:
			set_PWMdutycycle(3,0);
			set_PWMdutycycle(4,0);
			xDesiredSteps = steps;
			xStepElapsed = 0;
			break;
		case 2:
			set_PWMdutycycle(3,0);
			set_PWMdutycycle(12,0);
			yDesiredSteps = steps;
			yStepElapsed = 0;
			break;
		case 3:
			set_PWMdutycycle(4,0);
			set_PWMdutycycle(12,0);
			zDesiredSteps = steps;
			zStepElapsed = 0;
			break;
		default:
			break;

	}
	GPIOB->ODR &= ~(1U << stepperEN);
	countSteps = 1;

}

void oneRevolution(uint8_t microstepping,int axis){

	uint32_t steps_per_revolution = 200;
	uint32_t steps =0;

	if(microstepping ==0){
		microstepping = 1;
	}

	steps = steps_per_revolution * microstepping;
	move_axis_stepper_motor(axis,steps);

}

void timerCallback(int axis){
	if(countSteps){
		switch(axis){
			case 1:
				if (xDesiredSteps != 0){
					// Increment step count for axis Z
					xStepElapsed++;
					if (xStepElapsed >= xDesiredSteps) {
						// Perform necessary action when desired steps are reached
						GPIOB->ODR |= (1U << stepperEN);
						countSteps = 0;
					}
				}
				break;
			case 2:
				if (yDesiredSteps != 0){
					yStepElapsed++;
					if (yStepElapsed >= yDesiredSteps) {
						// Perform necessary action when desired steps are reached
						GPIOB->ODR |= (1U << stepperEN);
						countSteps = 0;
					}
				}
				break;
			case 3:
				if (zDesiredSteps != 0){
					// Increment step count for axis Z
					zStepElapsed++;
					if (zStepElapsed >= zDesiredSteps) {
						// Perform necessary action when desired steps are reached
						GPIOB->ODR |= (1U << stepperEN);
						countSteps = 0;
					}
				}
		        break;
		}
	}
}


void TIM3_IRQHandler(){
	if(TIM3->SR  & TIM_SR_UIF){
		timerCallback(3);
		TIM3->SR &= ~(TIM_SR_UIF); // Clear update interrupt flag
	}
}

void TIM4_IRQHandler(){
	if(TIM4->SR  & TIM_SR_UIF){
		timerCallback(2);
		TIM4->SR &= ~(TIM_SR_UIF); // Clear update interrupt flag
	}
}

void TIM8_BRK_TIM12_IRQHandler(){
	if(TIM12->SR  & TIM_SR_UIF){
		TIM12->SR &= ~(TIM_SR_UIF); // Clear update interrupt flag
		timerCallback(1);

	}
}

