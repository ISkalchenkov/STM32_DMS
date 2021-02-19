	GET	stm32f10x.s

STACK_SIZE		EQU 0x100
STACK_TOP		EQU	0x20000000 + STACK_SIZE			; Вершина стека
DELAY_VAL		EQU 100000							; Величина задержки

DG_PER_POINT_MUL	EQU (85 - (-55)) * 1000000 / 4095
DIVIDER				EQU 10000
	
	END