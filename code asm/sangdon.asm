
BASE DATA 30H 
MASK DATA 31H
PLUS DATA 32H
J DATA 33H

ORG 0000H

MAIN_LOOP:
    MOV BASE, #0FFH 
    MOV MASK, #0FFH
    MOV R4, #0 ;i                 

FOR_I:
    NECJ R4,#8, OK_I
    SJMP MAIN_LOOP
OK_I:
    MOV A, BASE
    CLR C
    RRC A
    MOV BASE, A
    MOV PLUS, #01H

    MOV A, #9
    CLR C
    SUBB A, R4     ; A = 9 - i
    MOV J , A

FOR_J:
    DJNZ J, OK_J
    SJMP END_J
OK_J:
    MOV A, PLUS 
    CPL A ; A = ~plus
    ANL A, MASK ; A = mask & ~plus
    MOV P2, A

    MOV A, PLUS 
    RL A 
    MOV PLUS, A
    LCALL DELAY200MS
    SJMP FOR_J
END_J: 
    MOV MASK, BASE 
    INC R4
    SJMP FOR_I

DELAY200MS:
    MOV R2, #200
D2:
    MOV R1, #200
D1:
    NOP
    NOP
    NOP
    DJNZ R1, D1
    DJNZ R2, D2
    RET

END
