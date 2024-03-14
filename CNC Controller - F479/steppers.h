#ifndef _STEPPERS_H_
#define _STEPPERS_H_

#include "mbed.h"

#define stepperEN_pin PB_9
#define xdir_pin PB_4
#define xstep_pin PB_15
#define ydir_pin PB_3
#define ystep_pin PB_8
#define zdir_pin PB_6
#define zstep_pin PB_5


void stepperInit(int frequency);




#endif