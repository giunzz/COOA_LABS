;Led sáng 3s rồi tự tắt (Led7seg hiện số 3 rồi đếm lên về 0 thì led đơn tắt)
SEGMENT_PORT EQU P0     
VALUE        DATA 30H     
LED_CTRL     BIT P2.0 

ORG 0000H
    SJMP MAIN

MAIN:
    MOV VALUE, #00H      ; start
	SETB LED_CTRL 
	

MAIN_LOOP:

    MOV DPTR, #TABLE
    MOV A, VALUE
    MOVC A, @A+DPTR      
    MOV SEGMENT_PORT, A  
	
    LCALL DELAY1S
    INC VALUE
	CLR LED_CTRL 
    MOV A, VALUE
    CJNE A, #04H, CONT     ; end
	
    MOV VALUE, #00H
	SETB LED_CTRL 

CONT:
    SJMP MAIN_LOOP
	
DELAY_200MS:
    MOV R2, #200
D1:
    MOV R1, #200
D2:
    DJNZ R1, D2
    DJNZ R2, D1
    RET

DELAY1S:
    MOV R3, #5
L1:
    LCALL DELAY_200MS
    DJNZ R3, L1
    RET


TABLE:
    DB 03FH
    DB 006H
    DB 05BH
    DB 04FH
    DB 066H
    DB 06DH
    DB 07DH
    DB 007H
    DB 07FH
    DB 06FH

END
