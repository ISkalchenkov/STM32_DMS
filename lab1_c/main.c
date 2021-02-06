#include "main.h"

int main() {
    port_init();

    uint16_t counter = 0;
    uint16_t reverse = 0;
    uint16_t button_is_ready = 1;
	
    while(1) {
        if (counter == 30)
            reverse = 1;
				
        if (counter == 0)
            reverse = 0;
				
        if (!button_is_ready) {
            button_is_ready = button_delay();
		} else {
            uint16_t prev = counter;
            counter = check_button(counter, reverse);
		
            if (prev != counter)
                button_is_ready = 0;
		}
        print_number(counter);
    }
}

void port_init() {
    // Включить тактирование портов GPIOA и GPIOB
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;

    // Сброс битов регистра конфигурации порта A   
    GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0);
    GPIOA->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_MODE1);
    GPIOA->CRL &= ~(GPIO_CRL_CNF2 | GPIO_CRL_MODE2);
    GPIOA->CRL &= ~(GPIO_CRL_CNF3 | GPIO_CRL_MODE3);
    GPIOA->CRL &= ~(GPIO_CRL_CNF4 | GPIO_CRL_MODE4);
    GPIOA->CRL &= ~(GPIO_CRL_CNF5 | GPIO_CRL_MODE5);
    GPIOA->CRL &= ~(GPIO_CRL_CNF6 | GPIO_CRL_MODE6);
    GPIOA->CRL &= ~(GPIO_CRL_CNF7 | GPIO_CRL_MODE7);

    // Сброс битов регистра конфигурации порта B 	
    GPIOB->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0);
    GPIOB->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_MODE1);
    
	// Настройка PA0 на вход
	// MODE 00 - input mode
	// CNF 01 - floating input
    GPIOA->CRL |= GPIO_CRL_CNF0_0;

    // Настройка PA1-PA7 и PB0-PB1 на выход
	// MODE 10 - output mode, max speed 2MHz
	// CNF 00 - push-pull
    GPIOA->CRL |= GPIO_CRL_MODE1_1;
    GPIOA->CRL |= GPIO_CRL_MODE2_1;
    GPIOA->CRL |= GPIO_CRL_MODE3_1;
    GPIOA->CRL |= GPIO_CRL_MODE4_1;
    GPIOA->CRL |= GPIO_CRL_MODE5_1;
    GPIOA->CRL |= GPIO_CRL_MODE6_1;
    GPIOA->CRL |= GPIO_CRL_MODE7_1;

    GPIOB->CRL |= GPIO_CRL_MODE0_1;
    GPIOB->CRL |= GPIO_CRL_MODE1_1;
}

uint16_t check_button(uint16_t counter, uint16_t reverse) {
    if (GPIOA->IDR & GPIO_IDR_IDR0) {
        if (reverse) {
            --counter;
        } else {
            ++counter;
        }
    }
    return counter;
}

void print_number(const uint16_t number) {
    uint16_t ones = number % 10;
    uint16_t dozens = number / 10;

    GPIOA->BSRR = RESET;
    match(dozens);
    GPIOB->BSRR = GPIO_BSRR_BS0;
   	delay(DELAY_VAL);
    GPIOB->BSRR = GPIO_BSRR_BR0;
    
    GPIOA->BSRR = RESET;
    match(ones);
    GPIOB->BSRR = GPIO_BSRR_BS1;
    delay(DELAY_VAL);
  	GPIOB->BSRR = GPIO_BSRR_BR1;
}

void match(const uint16_t pattern) {
    switch(pattern) {
        case 0:
            GPIOA->BSRR = ZERO;
            break; 
        case 1:
            GPIOA->BSRR = ONE;
            break;
        case 2:
            GPIOA->BSRR = TWO;
            break;
        case 3:
            GPIOA->BSRR = THREE;
            break;
        case 4:
            GPIOA->BSRR = FOUR;
            break;
        case 5:
            GPIOA->BSRR = FIVE;
            break;
        case 6:
            GPIOA->BSRR = SIX;
            break;
        case 7:
            GPIOA->BSRR = SEVEN;
            break;
        case 8:
            GPIOA->BSRR = EIGHT;
            break;
        case 9:
            GPIOA->BSRR = NINE;
            break;
        default:
            break;
    }
}

void delay(volatile uint32_t takts)
{
	while (takts--);
}

uint16_t button_delay() {
    static volatile uint16_t delay_val = BUTTON_DELAY_VAL;
    if (delay_val != 0) {
        --delay_val;
		return 0;
	} else {
        delay_val = BUTTON_DELAY_VAL;
		return 1;
	}
}
