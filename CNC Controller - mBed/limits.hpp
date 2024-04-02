#ifndef _LIMITS_HPP_
#define _LIMITS_HPP_


#include "mbed.h"

#define xlimit1_PIN     PE_0
#define xlimit2_PIN     PE_2
#define y1limit1_PIN    PE_3
#define y1limit2_PIN    PE_4
#define y2limit1_PIN    PE_5
#define y2limit2_PIN    PE_6
#define zlimit1_PIN     PE_7
#define zlimit2_PIN     PE_8


void limitsInit();
void softLimitCheck();




#endif