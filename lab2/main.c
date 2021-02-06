#include "main.h"

volatile static uint16_t counter;
volatile static uint16_t reverse;

int main() {
    port_init();
    TIM2_init();
    button_init();

    while(1);
}

void port_init() {
    // Включение тактирования портов GPIOA, GPIOB и альтернативных функций
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;

    // Сброс битов регистра конфигурации порта A   
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

void button_init() {
    // Включение тактирования порта A и альтернативных функций
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

    // Сброс битов регистра конфигурации порта A   
    GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0);
    
    // Настройка PA0 на вход
	// MODE 00 - input mode
	// CNF 01 - floating input
    GPIOA->CRL |= GPIO_CRL_CNF0_0;
    
    AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PA;   // Конфигурация мультиплексора канала EXTI0
    EXTI->RTSR |= EXTI_RTSR_TR0;                // Включение детектора нарастающего края импульса
    EXTI->IMR |= EXTI_IMR_MR0;                  // Разрешаем прерывания канала EXTI0
    
    NVIC_EnableIRQ(EXTI0_IRQn);                 // Разрешаем прерывание в контроллере прерываний
    NVIC_SetPriority(EXTI0_IRQn, 1);
}

void TIM2_init() {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;         // Включение тактирования TIM2
    
    TIM2->PSC = 1200 - 1;                       // Предделитель частоты (72МГц/1200 = 60кГц)
    TIM2->ARR = 240 - 1;                        // Значение автоматической перезагрузки (240/60кГц = 0.004с)
    TIM2->DIER |= TIM_DIER_UIE;                 // Разрешение генерирования прерывания по событию UEV
    TIM2->CR1 |= TIM_CR1_CEN;                   // Включение счетчика
    
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn, 0);
}

static void change_counter(void) {
    if (counter == 30)
        reverse = 1;
    if (counter == 0)
        reverse = 0;
    
    if (reverse)
        --counter;
    else
        ++counter;
}

void EXTI0_IRQHandler() {
    delay(1000000);                             // Защита от дребезга контактов
    change_counter();
    
    while(GPIOA->IDR & GPIO_IDR_IDR0) {         
        if (GPIOA->IDR & GPIO_IDR_IDR0) {       // Возможность счета зажатием кнопки
            delay(1500000);
            change_counter();
        }
    }
    EXTI->PR |= EXTI_PR_PR0;                    // Сброс бита ожидания обработки прерывания
    delay(1000000);                             // Защита от дребезга контактов
}

void TIM2_IRQHandler() {
    TIM2->SR &= ~TIM_SR_UIF;                    // Сброс бита ожидания прерывания по событию UEV
    static uint16_t flag = 1;
    GPIOA->BSRR = RESET;
    GPIOB->BSRR = GPIO_BSRR_BR0 | GPIO_BSRR_BR1;
    if (flag) {
        uint16_t dozens = counter / 10;
        match(dozens);
        GPIOB->BSRR = GPIO_BSRR_BS0;
        flag = 0;
    } else {
        uint16_t ones = counter % 10;
        match(ones);
        GPIOB->BSRR = GPIO_BSRR_BS1;
        flag = 1;
    }
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

void delay(volatile uint32_t takts) {
	while (takts--);
}
