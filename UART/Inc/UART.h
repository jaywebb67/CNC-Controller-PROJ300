/*
 * UART.h
 *
 *  Created on: Dec 17, 2023
 *      Author: jaywe
 */

#ifndef UART_H_
#define UART_H_


#include <stdint.h>
#include "CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h"


#define GPIODEN			(1U<<3)
#define UART3EN			(1U<<18)

#define CR1_TE			(1U<<3)
#define CR1_RE			(1U<<2)
#define CR1_RXNEIE		(1U<<2)
#define CR1_UE			(1U<<13)
#define SR_TXE			(1U<<7)
#define SR_RXNE			(1U<<5)


#define UART_BAUDRATE	115200
#define CLK				16000000

void uart3_tx_init(void);

#endif /* UART_H_ */
