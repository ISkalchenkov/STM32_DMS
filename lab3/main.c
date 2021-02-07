#include "main.h"

volatile static uint16_t counter;
volatile static uint16_t reverse;

static char RxBuffer[RX_BUF_SIZE];

int main() {
    port_init();
    button_init();
    TIM2_init();
    USART1_init();
    
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
}

void USART1_init(void) {
    // Включение тактирования GPIOA и USART1
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;
    
    // Настройка PA9 на выход, режим альтернативной функции
    // Тип выхода push-pull, max speed 2MHz
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= (GPIO_CRH_MODE9_1 | GPIO_CRH_CNF9_1);

    // Настройка PA10 на вход, floating input 
    GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |= GPIO_CRH_CNF10_0;
    
    /*
    Настройка скорости передачи данных(115200)
    Частота шины APB2 - 72МГц
    
    USART_DIV = 72000000/(16*115200) = 39,0625
    Целая часть: 0d39 = 0x27
    Дробная часть: 16*0,0625 = 1
    Итог: 0x271
    */
    USART1->BRR = 0x271;
    
    // Включение модуля USART, передатчика и приемника
    USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
    // Включение прерывания по приему данных
    USART1->CR1 |= USART_CR1_RXNEIE;
    
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_SetPriority(USART1_IRQn, 1);
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
    
    while(GPIOA->IDR & GPIO_IDR_IDR0) {         // Возможность счета зажатием кнопки
        if (GPIOA->IDR & GPIO_IDR_IDR0) {
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

void USART_SendChar(USART_TypeDef *const USART, const char c) {
    USART->DR = c;
    while(!(USART->SR & USART_SR_TC));
}

void USART_Transmit(USART_TypeDef *const USART, char *buf) {
    for (size_t i = 0; i < strlen(buf); ++i) {
        USART_SendChar(USART, buf[i]);
    }
}

void USART1_IRQHandler() {
    char temp;
    if (USART1->SR & USART_SR_RXNE) {
        temp = (char)USART1->DR;
        char prev = RxBuffer[strlen(RxBuffer) - 1];
        size_t buf_length = strlen(RxBuffer);
        RxBuffer[buf_length++] = temp;
        if ((prev == '\r' && temp == '\n')
            || (prev == '\n' && temp == '\r')
            || buf_length == RX_BUF_SIZE) {
                Command_Handler();
                memset(RxBuffer, 0, sizeof(RxBuffer));
        }
    }
}

void Command_Handler() {
    size_t command_length = strlen(RxBuffer) - END_CONTROL_CHARACTERS;
    if ((strncmp(RxBuffer, "*IDN?", strlen("*IDN?")) == 0)
        && (command_length == strlen("*IDN?"))) {
            USART_Transmit(USART1, "Skalchenkov_Slepov_IU4-73\r\n");
            return;
    }
    if ((strncmp(RxBuffer, "INC", strlen("INC")) == 0)
        && (command_length == strlen("INC"))) {
            if (counter == 30) {
                USART_Transmit(USART1, "Invalid command, counter is 30\r\n");
                return;
            }
            ++counter;
            OK();
            return;
    }
    if ((strncmp(RxBuffer, "DEC", strlen("DEC")) == 0)
        && (command_length == strlen("DEC"))) {
            if (counter == 0) {
                USART_Transmit(USART1, "Invalid command, counter is 0\r\n");
                return;
            }
            --counter;
            OK();
            return;
    }
    if ((strncmp(RxBuffer, "SET ", strlen("SET ")) == 0)                        // Template: SET dd
        && (command_length == strlen("SET dd"))) {                              // 00 <= dd <= 30
            char *end_ptr;
            char *begin_ptr = RxBuffer + strlen("SET ");
            long int dd = strtol(begin_ptr, &end_ptr, 10);
            if (dd <= 30 && dd >= 0 && end_ptr == (begin_ptr + strlen("dd"))) {
                counter = (uint16_t)dd;
                OK();
            } else {
                INVALID_PARAM();
            }
            return;
    }
    UNKNOWN_COMMAND();
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
