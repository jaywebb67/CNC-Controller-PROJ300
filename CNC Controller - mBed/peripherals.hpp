#ifndef _PERIPHERALS_HPP_
#define _PERIPHERALS_HPP_

#include "mbed.h"
#include "system.hpp"

#define safetydoorPin PF_2
#define holdButtonPin PF_1
#define stopButtonPin PD_0


#define STATE_hold bit(0)
#define STATE_safetyDoor bit(1)

extern InterruptIn stopButton;
extern InterruptIn holdButton;
extern InterruptIn safetyDoor;

extern uint8_t holdFlag;

void peripheralsInit();


#endif