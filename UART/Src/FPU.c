/*
 * FPU.c
 *
 *  Created on: Feb 29, 2024
 *      Author: jaywe
 */


#include "FPU.h"




void enableFPU(){

	SCB->CPACR |= (1U << 20)|(1U << 21)|(1U << 22)|(1U << 23);
}
