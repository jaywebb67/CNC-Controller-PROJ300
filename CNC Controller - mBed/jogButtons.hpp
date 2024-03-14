#ifndef _JOGBUTTONS_H_
#define _JOGBUTTONS_H_


#include "mbed.h"

#define xJog_plus_pin PC_0
#define xJog_minus_pin PC_1

#define yJog_plus_pin PC_2
#define yJog_minus_pin PC_3

#define zJog_plus_pin PC_4
#define zJog_minus_pin PC_6

#define jogIncrement_MM 0.01
#define distance_per_step_mm 0.0025

void jogInit();
void jogEnable(bool state);





#endif