#ifndef _JOGBUTTONS_H_
#define _JOGBUTTONS_H_


#include "mbed.h"
#include "steppers.hpp"


#define xJog_plus_pin PD_0
#define xJog_minus_pin PD_1

#define yJog_plus_pin PD_2
#define yJog_minus_pin PD_3

#define zJog_plus_pin PD_4
#define zJog_minus_pin PD_5

#define jogIncrement_MM 0.01
#define distance_per_step_mm 0.0025

#define steps_per_increment (jogIncrement_MM/distance_per_step_mm)


#define switchBounce_delayms 10 

void jogInit();
void jogEnable(bool state);





#endif