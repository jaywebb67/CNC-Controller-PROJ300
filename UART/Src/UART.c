/*
 * UART.c
 *
 *  Created on: Dec 17, 2023
 *      Author: jaywe
 */

#include "UART.h"





static uint16_t compute_uart_bd(uint32_t periph_clk, uint32_t baudrate);
static void uart_set_baudrate(uint32_t periph_clk, uint32_t baudrate);
static void uart3_write(int ch);
static int uart_get(void);

int __io_putchar(int ch){
	uart3_write(ch);
	return ch;
}

int __io_getchar(void){

	int ch;
	do {
	ch = uart_get();
	}
	while (ch == -1);
	uart3_write(ch); //Echo char
	return (ch);

}



void uart3_tx_init(void)
{
	/*************Configure UART GPIO pin********************/
	/*1.Enable clock access to GPIOA*/
	RCC->AHB1ENR |= GPIODEN;

	/*2.Set PD8 mode to alternate function mode*/
	GPIOD->MODER &= ~(1U<<16);
	GPIOD->MODER |=	 (1U<<17);
	//PD9
	GPIOD->MODER &= ~(1U<<18);
	GPIOD->MODER |=	 (1U<<19);

	/*3.Set PA2 alternate function function type to AF7(UART2_TX)*/
	GPIOD->AFR[1] |= (1U<<0);
	GPIOD->AFR[1] |= (1U<<1);
	GPIOD->AFR[1] |= (1U<<2);
	GPIOD->AFR[1] &= ~(1U<<3);

	GPIOD->AFR[1] |= (1U<<4);
	GPIOD->AFR[1] |= (1U<<5);
	GPIOD->AFR[1] |= (1U<<6);
	GPIOD->AFR[1] &= ~(1U<<7);

	/*************Configure UART Module********************/
	/*4. Enable clock access to UART2*/
	RCC->APB1ENR |= UART3EN;

	USART3->CR1 &= ~(CR1_UE);

	/*5. Set baudrate*/
	uart_set_baudrate(CLK,UART_BAUDRATE);

	/*6. Set transfer direction*/
	USART3->CR1 = (CR1_TE | CR1_RE);

	USART3->CR1 |= CR1_RXNEIE;

	NVIC_EnableIRQ(USART3_IRQn);

	/*7. Enable uart module*/
	USART3->CR1 |= CR1_UE;

}

static void uart3_write(int ch)
{
	/*Make sure the transmit data register is empty*/
	while(!(USART3->SR & SR_TXE)){}

	/*Write to transmit data register*/
	USART3->DR = (ch & 0xFF);
}

static int uart_get(void){

	if (((USART3->SR & SR_RXNE) != 0) &&((USART3->CR1 & CR1_RXNEIE) != 0)){ //read data Register full?
		return USART3->DR;
	}
	else {
		return -1;
	}
}

static uint16_t compute_uart_bd(uint32_t periph_clk, uint32_t baudrate)
{
	return ((periph_clk +( baudrate/2U ))/baudrate);
}


static void uart_set_baudrate(uint32_t periph_clk, uint32_t baudrate)
{
	USART3->BRR  = compute_uart_bd(periph_clk,baudrate);
}

void USART3_IRQHandler(void){

	uart_get();

}

