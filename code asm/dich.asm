ORG 0000H

BEGIN_ENTRY:
    MOV A, #0FEH     ; 1111 1110  LED P2.0 sáng 

MAIN_LOOP:
    MOV P2, A        
    LCALL DELAY1S

    RL A             

    CJNE A, #0FFH, CONT ; ss P2.7 = 1 -> quay lại P2.0
    MOV A, #0FEH     

CONT:
    SJMP MAIN_LOOP
    
DELAY1S:
    MOV R3, #5
L1:
    LCALL DELAY200ms
    DJNZ R3, L1
    RET

DELAY200ms:
    MOV R2, #200
L2:
    MOV R1, #200
L1_1:
    NOP
    NOP
    NOP
    DJNZ R1, L1_1
    DJNZ R2, L2
    RET

END
