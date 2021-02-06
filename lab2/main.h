#ifndef MAIN_H
#define MAIN_H

#include <stm32f10x.h>

#define ZERO     GPIO_BSRR_BS7
#define ONE      GPIO_BSRR_BS1 | GPIO_BSRR_BS4 | GPIO_BSRR_BS5 | GPIO_BSRR_BS6 | GPIO_BSRR_BS7
#define TWO      GPIO_BSRR_BS3 | GPIO_BSRR_BS6
#define THREE    GPIO_BSRR_BS5 | GPIO_BSRR_BS6
#define FOUR     GPIO_BSRR_BS1 | GPIO_BSRR_BS4 | GPIO_BSRR_BS5
#define FIVE     GPIO_BSRR_BS2 | GPIO_BSRR_BS5
#define SIX      GPIO_BSRR_BS2
#define SEVEN    GPIO_BSRR_BS4 | GPIO_BSRR_BS5 | GPIO_BSRR_BS6 | GPIO_BSRR_BS7
#define EIGHT    0
#define NINE     GPIO_BSRR_BS5
#define RESET    GPIO_BSRR_BR1 | GPIO_BSRR_BR2 | GPIO_BSRR_BR3 | GPIO_BSRR_BR4 | GPIO_BSRR_BR5 | GPIO_BSRR_BR6 | GPIO_BSRR_BR7

void EXTI0_IRQHandler(void);
void TIM2_IRQHandler(void);

void port_init(void);
void button_init(void);
void TIM2_init(void);

void match(const uint16_t pattern);
void delay(volatile uint32_t takts);

#endif // MAIN_H
