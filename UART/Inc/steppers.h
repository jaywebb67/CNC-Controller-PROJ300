/*
 * steppers.h
 *
 *  Created on: Feb 21, 2024
 *      Author: jaywe
 */

#ifndef STEPPERS_H_
#define STEPPERS_H_

#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>


#define x_step			15
#define x_dir			4
#define y_step			8
#define y_dir			3
#define z_step			5
#define z_dir			6

#define stepperEN		9


#define APB1_TIM_FREQ	90000000

extern volatile uint32_t zStepElapsed;

extern volatile uint32_t yStepElapsed;

extern volatile uint32_t xStepElapsed;

extern volatile uint32_t xDesiredSteps;

extern volatile uint32_t yDesiredSteps;

extern volatile uint32_t zDesiredSteps;

extern volatile bool countSteps;

	//PB15 x-step TIM12 Channel 2 (AF9) PB4 x-dir,
	//PB3 y-dir PB8 y-step TIM4 channel 3 (AF2),
	//PB5 z-step TIM3 channel 2 (AF2) PB6 z-dir

void init_stepper(unsigned int F_pwm);
void stepper_init(uint32_t frequency, uint8_t d);
void oneRevolution(uint8_t microstepping,int axis);
void move_axis_stepper_motor(int axis, uint32_t steps);
void setPWMfrequency(uint8_t timr, uint32_t frequency);
void set_PWMdutycycle(uint8_t timr, uint32_t d);




#endif /* STEPPERS_H_ */
