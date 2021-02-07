	PRESERVE8							; 8-битное выравнивание стека
	THUMB								; Режим Thumb (AUL) инструкций

	GET	config.s						; include-файлы	

	AREA RESET, CODE, READONLY

	; Таблица векторов прерываний
	DCD STACK_TOP						; Указатель на вершину стека
	DCD Reset_Handler					; Вектор сброса
	
	ENTRY								; Точка входа в программу

Reset_Handler	PROC					; Вектор сброса
	EXPORT Reset_Handler

	MOV32	R0, PERIPH_BB_BASE + \
			(RCC_APB2ENR-PERIPH_BASE) * 32 + \
			2 * 4						; вычисляем адрес для BitBanding 2-го бита регистра RCC_APB2ENR
										; BitAddress = BitBandBase + (RegAddr * 32) + BitNumber * 4
	MOV		R1, #1						; включаем тактирование порта A (в 2-й бит RCC_APB2ENR пишем '1')
	STR		R1, [R0]					; загружаем это значение
		
	MOV32	R0, GPIOA_CRL				; адрес порта GPIOA_CRL
	MOV		R1, #2_0100					; 4-битная маска настроек для Input mode (floating input)
	LDR		R2, [R0]					; считать порт
	BFI		R2, R1, #0, #4				; скопировать биты маски в позицию PIN0
	
	MOV		R1, #2_0010					; 4-битная маска настроек для Output mode, max speed 2MHz
	BFI		R2, R1, #4, #4				; скопировать биты маски в позицию PIN1
	BFI		R2, R1, #8, #4				; скопировать биты маски в позицию PIN2
	BFI		R2, R1, #12, #4				; скопировать биты маски в позицию PIN3
	BFI		R2, R1, #16, #4				; скопировать биты маски в позицию PIN4
	BFI		R2, R1, #20, #4				; скопировать биты маски в позицию PIN5
	BFI		R2, R1, #24, #4				; скопировать биты маски в позицию PIN6
	BFI		R2, R1, #28, #4				; скопировать биты маски в позицию PIN7
	
	STR		R2, [R0]					; загрузить результат в регистр настройки порта A
	
	MOV32	R0, PERIPH_BB_BASE + \
			(RCC_APB2ENR-PERIPH_BASE) * 32 + \
			3 * 4						; вычисляем адрес для BitBanding 3-го бита регистра RCC_APB2ENR
										; BitAddress = BitBandBase + (RegAddr * 32) + BitNumber * 4
	MOV		R1, #1						; включаем тактирование порта B (в 3-й бит RCC_APB2ENR пишем '1')
	STR		R1, [R0]					; загружаем это значение
	
	MOV32	R0, GPIOB_CRL				; адрес порта GPIOB_CRL
	MOV		R1, #2_0010					; 4-битная маска настроек для Output mode, max speed 2MHz
	LDR		R2, [R0]					; считать порт
	BFI		R2, R1, #0, #4				; скопировать биты маски в позицию PIN0
	BFI		R2, R1, #4, #4				; скопировать биты маски в позицию PIN1

	STR		R2, [R0]					; загрузить результат в регистр настройки порта B

	MOV32	R0, GPIOA_BSRR				; адрес регистра BSRR порта A
	MOV32	R1, GPIOB_BSRR				; адрес регистра BSRR порта B
	MOV32	R2, GPIOA_IDR				; адрес порта входных сигналов
	
	MOV		R3, #0						; регистр для счета
	MOV		R4, #1						; флаг готовности кнопки
	MOV		R5, #BUTTON_DELAY_VAL		; задержка кнопки

loop									; бесконечный цикл
	CMP		R3, #31						; обнулить счетчик после 30
	MOVEQ	R3, #0 
	
	BL		print_number
	
	CMP		R4, #1						; button is ready?
	BEQ		button_is_ready

	CMP		R4, #0
	BEQ		button_is_not_ready

	B		loop

button_is_ready
	MOV		R6, R3						; предыдущее значение
	BL		check_button
	CMP		R6, R3
	MOVNE	R4, #0
	
	B		loop

button_is_not_ready
	BL		button_delay
	
	B		loop

	ENDP

delay		PROC						; Подпрограмма задержки
	PUSH	{R0}						; Загружаем в стек R0, т.к. его значение будем менять

	MOV32	R0, #DELAY_VAL				; псевдоинструкция Thumb (загрузить константу в регистр)
delay_loop
	SUBS	R0, #1						; SUB с установкой флагов результата
	IT		NE
	BNE		delay_loop					; переход, если Z==0 (результат вычитания не равен нулю)

	POP		{R0}						; Выгружаем из стека R0
	BX		LR							; выход из подпрограммы (переход к адресу в регистре LR - вершина стека)
	ENDP
	
check_button	PROC					; инкрементирует регистр R3, если кнопка нажата
	PUSH	{R0}
	
	LDR		R0, [R2]					; считать порт входных сигналов 
	ANDS 	R0, #GPIO_IDR_IDR0
	ADDNE 	R3, #1						; инкремент, если кнопка нажата
		
	POP		{R0}
	BX		LR
	ENDP
	
split_number	PROC					; разделить регистр R3 на десятки и единицы
	MOV		R5, #10						; в R4 записываются десятки, в R5 - единицы
	UDIV	R4, R3, R5					; R4 = R3 / R5
	MLS	R5, R4, R5, R3					; R5 = R3 - R4 x R5 		

	BX		LR
	ENDP

button_delay	PROC					; задержка на считывание статуса кнопки
	CMP		R5, #0						; В R4 записывается результат готовности к считыванию
	ITTEE	NE							; В R5 задается величина задержки
	MOVNE	R4, #0
	SUBNE	R5, #1
	MOVEQ	R4, #1
	MOVEQ	R5, #BUTTON_DELAY_VAL

	BX		LR
	ENDP

print_number	PROC					; выводит на двухразрядный семисегментный индикатор число, записанное в R3 
	PUSH	{R2, R4, R5, LR}						
	
	BL		split_number
	
	MOV32	R2, #RESET_LEDS
	STR		R2, [R0]
	
	MOV		R2, R4
	BL		match
	STR		R2, [R0]

	MOV		R2, #GPIO_BSRR_BS0
	STR		R2, [R1]

	BL		delay
	MOV32	R2, #GPIO_BSRR_BR0			; погасить индикатор
	STR		R2, [R1]
	
	MOV32	R2, #RESET_LEDS
	STR		R2, [R0]
	
	MOV		R2, R5
	BL		match
	STR		R2, [R0]

	MOV		R2, #GPIO_BSRR_BS1
	STR		R2, [R1]

	BL		delay
	MOV32	R2, #GPIO_BSRR_BR1			; погасить индикатор
	STR		R2, [R1]

	POP		{R2, R4, R5, LR}
	BX		LR

match	
	CMP		R2, #0
	ITT		EQ
	MOVEQ	R2, #ZERO
	BXEQ	LR

	CMP		R2, #1
	ITT		EQ
	MOVEQ	R2, #ONE
	BXEQ	LR
	
	CMP		R2, #2
	ITT		EQ
	MOVEQ	R2, #TWO
	BXEQ	LR

	CMP		R2, #3
	ITT		EQ
	MOVEQ	R2, #THREE
	BXEQ	LR

	CMP		R2, #4
	ITT		EQ
	MOVEQ	R2, #FOUR
	BXEQ	LR

	CMP		R2, #5
	ITT		EQ
	MOVEQ	R2, #FIVE
	BXEQ	LR

	CMP		R2, #6
	ITT		EQ
	MOVEQ	R2, #SIX
	BXEQ	LR

	CMP		R2, #7
	ITT		EQ
	MOVEQ	R2, #SEVEN
	BXEQ	LR

	CMP		R2, #8
	ITT		EQ
	MOVEQ	R2, #EIGHT
	BXEQ	LR
	
	CMP		R2, #9
	ITT		EQ
	MOVEQ	R2, #NINE
	BXEQ	LR

	BX		LR

	ENDP

    END
	
