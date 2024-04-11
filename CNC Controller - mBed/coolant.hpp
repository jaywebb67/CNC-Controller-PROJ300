#ifndef _COOLANT_HPP_
#define _COOLANT_HPP_



#include "mbed.h"
#include <cstdint>


#define floodPin_No 5
#define mistPin_No 10


#define coolantFlood_PIN PF_5
#define coolantMist_PIN PF_10


void coolantInit();

void coolantEnable(uint8_t type);


#endif