#ifndef _SPINDLE_HPP_
#define _SPINDLE_HPP_

#include "mbed.h"

#define spindleA_pin PA_5
#define spindleB_pin PA_3
#define spindleEN_pin PA_4

#define spindleAccel 0.001

extern PwmOut spindleA;

extern PwmOut spindleB;

extern DigitalOut spindleEN;



void spindleInit();
void enableSpindle(bool EN, bool dir);

void setSpindleSpeed(short speed);














#endif