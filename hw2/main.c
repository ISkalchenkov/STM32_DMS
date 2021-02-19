#include "main.h"

static char RxBuffer[RX_BUF_SIZE];

int main() {
    USART1_init();
    ADC1_init();
    
    while(1);
}

void ADC1_init() {
    // Включение тактирования GPIOB и ADC1
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_ADC1EN;
    
    // Настройка PB0(ain8). Analog input
    GPIOB->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
    // Настройка времени измерения(239.5 cycles)
    ADC1->SMPR2 |= ADC_SMPR2_SMP8;
    // Преобразование по внешнему событию SWSTART
    ADC1->CR2 |= ADC_CR2_EXTSEL | ADC_CR2_EXTTRIG;
    ADC1->CR2 |= ADC_CR2_ADON;
    delay(10);
    // Запуск калибровки
    ADC1->CR2 |= ADC_CR2_CAL;
    while(ADC1->CR2 & ADC_CR2_CAL);
}

int16_t make_conversation(uint8_t channel) {
    if (channel >= NUMBER_OF_CHANNELS)
        return -1;
    
    ADC1->SQR3 = channel;
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while(!(ADC1->SR & ADC_SR_EOC));
    
    return (int16_t)ADC1->DR;
}

float read_temp(uint8_t channel) {
    int16_t adc_result = make_conversation(channel);
    if (adc_result == -1)
        return -1;
    float dg_per_point = (float)(HIGHT_BOUNDARY_TEMP - LOW_BOUNDARY_TEMP)
        / (float)(ADC_BIT_CAPACITY - 1);
    float temperature = (dg_per_point * adc_result) + (float)LOW_BOUNDARY_TEMP;
    return roundf(temperature * 10) / 10;
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

void USART_SendChar(USART_TypeDef *const USART, const char c) {
    USART->DR = c;
    while(!(USART->SR & USART_SR_TC));
}

void USART_Transmit(USART_TypeDef *const USART, const char *buf) {
    for (size_t i = 0; i < strlen(buf); ++i) {
        USART_SendChar(USART, buf[i]);
    }
}

void USART1_IRQHandler() {
    char temp;
    if (USART1->SR & USART_SR_RXNE) {
        temp = (char)USART1->DR;
        size_t buf_length = strlen(RxBuffer);
        RxBuffer[buf_length++] = temp;
        if (temp == '\r' || buf_length == RX_BUF_SIZE) {
            Command_Handler();
            memset(RxBuffer, 0, sizeof(RxBuffer));
        }
    }
}

void Command_Handler() {
    size_t command_length = strlen(RxBuffer) - END_CONTROL_CHARACTERS;
    if ((strncmp(RxBuffer, "TEMPER?", strlen("TEMPER?")) == 0)
        && (command_length == strlen("TEMPER?"))) {
            float temperature = read_temp(EIGHT_CHANNEL);
            char message[64];
            snprintf(message, sizeof(message) - sizeof('\0'),
                "%1.2f\r", (double)temperature);
            USART_Transmit(USART1, message);
            return;
    }
    
    UNKNOWN_COMMAND();
}

void delay(volatile uint32_t takts) {
	while (takts--);
}
