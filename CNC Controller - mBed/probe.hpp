#ifndef _PROBE_HPP_
#define _PROBE_HPP_

#include "mbed.h"

#define probe_PIN PF_7

#define PROBE_OFF 0
#define PROBE_ACTIVE 1


extern InterruptIn probe;
extern uint8_t probeFlag;

void probeInit();
bool probeTool();



#endif