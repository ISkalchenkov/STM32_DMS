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
	
	MOV 	R1, #500					; Для тестирования функции read_temp
	BL		read_temp
	
loop									; Бесконечный цикл
	B		loop

	ENDP

read_temp	PROC						; читает значение регистра ADC1_DR и преобразует его к градусам цельсия
	PUSH	{R0, R1, LR}				; 0 = -55 град. цел.; 4095 = 85 град. цел.

	;MOV32	R0, ADC1_DR					; Раскомментировать для работы с ADC_DR
	;LDR	R1, [R0]

	MOV		R0, #DG_PER_POINT_MUL
	MUL		R1, R0						; R1 = 17094000

	MOV		R0, #DIVIDER				
	UDIV	R1, R0						; R1 = 1709

	BL 		split_number				; R0 = 170, R1 = 9
	BL		round_up					; R0 = 171
	
	MOV		R1, R0						; R1 = 171
	BL 		split_number				; R0 = 17, R1 = 1
	
	BL 		convert_to_fixed_point		; R5 = 1000 0000 0010 0101 0000 0000 0000 1001 = -37.9
										; bit 31 - minus
	POP		{R0, R1, LR}				; bits [16;30] - integer part = 37
	BX		LR							; bits [0;15] - fraction part = 9
	ENDP

split_number	PROC					; отделяет от числа, лежащего в регистре R1, младшую цифру
	PUSH 	{R2, LR}

	MOV		R2, #10						; R2 = 10
	UDIV	R0, R1, R2					; R0 = 7001 / 10 = 700
	MUL		R2, R0, R2					; R2 = 700 * 10 = 7000
	SUB		R1, R2 						; R1 = 7001 - 7000 = 1 

	POP 	{R2, LR}
	BX 		LR
	ENDP

round_up	PROC						; округляет число в зависимости от дробного значения
	PUSH	{LR, R2}					; в R0 целое значение, в R1 - дробное.
										; Изменяет в R0

	CMP		R1, #5
	IT		CS
	ADDCS	R0, #1

	POP 	{LR, R2}
	BX		LR
	
	ENDP
	
convert_to_fixed_point	PROC			; конвертирует в число с плавающей точкой, записывает в R5
	PUSH	{LR, R1, R2}

	MOV		R5, #0
	
	CMP		R0, #55
	IT		CS
	BCS		positive
	
	B		negative
	
positive
	BFI		R5, R1, #0, #4				; 2^4 = 16
	SUB		R1, R0, #55
	BFI		R5, R1, #16, #7				; 2^7 = 128
	BCS		convert_end
	
negative
	CMP		R1, #0
	ITTTE	NE
	MOVNE	R2, #10
	SUBNE	R1, R2, R1
	MOVNE	R2, #54
	MOVEQ	R2, #55
	
	BFI		R5, R1, #0, #4				; 2^4 = 16
	
	SUB		R1, R2, R0
	BFI		R5, R1, #16, #7
	MOV		R2, #1
	BFI		R5, R2, #31, #1
	
convert_end	
	POP		{LR, R1, R2}
	BX		LR
	
	ENDP
		
	END
