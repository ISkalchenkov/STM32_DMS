#ifndef MAIN_H
#define MAIN_H

#include <stm32f10x.h>

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define RX_BUF_SIZE             256

#define END_CONTROL_CHARACTERS  1

#define NUMBER_OF_CHANNELS      16

#define LOW_BOUNDARY_TEMP     (-55.0)
#define HIGHT_BOUNDARY_TEMP     85.0

#define ADC_BIT_CAPACITY        4096

#define EIGHT_CHANNEL           8

#define UNKNOWN_COMMAND()   USART_Transmit(USART1, "Unknown command\r\n")


void USART1_IRQHandler(void);

void USART1_init(void);
void ADC1_init(void);

void USART_SendChar(USART_TypeDef *const USART, const char c);
void USART_Transmit(USART_TypeDef *const USART, const char *buf);
void Command_Handler(void);
int16_t make_conversation(uint8_t channel);
float read_temp(uint8_t channel);

void delay(volatile uint32_t takts);

#endif // MAIN_H
