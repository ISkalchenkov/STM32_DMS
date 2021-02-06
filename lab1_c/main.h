#ifndef MAIN_H
#define MAIN_H

#include <stm32f10x.h>

#define DELAY_VAL           10000
#define BUTTON_DELAY_VAL    30

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

void port_init(void);

uint16_t check_button(uint16_t counter, uint16_t reverse);
void print_number(const uint16_t number);
void match(const uint16_t pattern);

void delay(volatile uint32_t takts);
uint16_t button_delay(void);

#endif // MAIN_H
