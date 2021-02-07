	GET	stm32f10x.s

STACK_SIZE			EQU 0x100						; Размер стека
STACK_TOP			EQU	0x20000000 + STACK_SIZE		; Вершина стека
DELAY_VAL			EQU 10000						; Величина задержки
BUTTON_DELAY_VAL	EQU 20							; Величина задержки кнопки

ZERO		EQU	GPIO_BSRR_BS7
ONE			EQU GPIO_BSRR_BS1 | GPIO_BSRR_BS4 | GPIO_BSRR_BS5 | GPIO_BSRR_BS6 | GPIO_BSRR_BS7
TWO			EQU GPIO_BSRR_BS3 | GPIO_BSRR_BS6
THREE		EQU	GPIO_BSRR_BS5 | GPIO_BSRR_BS6
FOUR		EQU GPIO_BSRR_BS1 | GPIO_BSRR_BS4 | GPIO_BSRR_BS5
FIVE		EQU GPIO_BSRR_BS2 | GPIO_BSRR_BS5
SIX			EQU	GPIO_BSRR_BS2
SEVEN		EQU	GPIO_BSRR_BS4 | GPIO_BSRR_BS5 | GPIO_BSRR_BS6 | GPIO_BSRR_BS7
EIGHT		EQU	0
NINE		EQU GPIO_BSRR_BS5
RESET_LEDS	EQU GPIO_BSRR_BR1 | GPIO_BSRR_BR2 | GPIO_BSRR_BR3 | GPIO_BSRR_BR4 | GPIO_BSRR_BR5 | GPIO_BSRR_BR6 | GPIO_BSRR_BR7
	
	END
