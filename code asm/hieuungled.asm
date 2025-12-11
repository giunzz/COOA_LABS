ORG 0000H

MASK    EQU 30H     ; fixed bits on the left for effect 2
TEMP    EQU 31H     ; temp register for effect 2

MAIN:
LOOP_MAIN:
    ACALL EFFECT_SHIFT       ; 1. led shift
    ACALL EFFECT_SANG_DON    ; 2. waterfall fill
    ACALL EFFECT_TAT_DAN     ; 3. fade out
    ACALL EFFECT_CHOP_TAT    ; 4. blink all
    ACALL EFFECT_SANG_DAN    ; 5. fade in
    SJMP  LOOP_MAIN

SHOW_P2:
    MOV  B, A
    CPL  A           ; invert bits for active-low LEDs
    MOV  P2, A
    MOV  A, B
    RET

; 1. EFFECT_SHIFT: 
EFFECT_SHIFT:
    MOV  A, #01H     ; start with bit 0
    MOV  R7, #16     ; number of steps

SHIFT_LOOP:
    ACALL SHOW_P2
    ACALL DELAY200MS

    RL   A           ; rotate left
    JNZ  SHIFT_CONT
    MOV  A, #01H     ; if A became 00H, restart
SHIFT_CONT:
    DJNZ R7, SHIFT_LOOP
    RET


; 2. EFFECT_SANG_DON
EFFECT_SANG_DON:

    ; step 0: all OFF (logic)
    MOV  A, #00H
    ACALL SHOW_P2
    ACALL DELAY200MS

    ; stage 0: single running bit
    MOV  A, #01H
    MOV  R7, #8

STAGE0_LOOP:
    ACALL SHOW_P2
    ACALL DELAY200MS

    RL   A           ; move bit to the left
    DJNZ R7, STAGE0_LOOP

    ; stages 1..7: fixed left block + running bit on the right
    MOV  MASK, #080H ; first fixed block: 1000 0000
    MOV  R4,   #1    ; stage = 1

STAGE_OUTER:
    ; inner_count = 8 - stage
    CLR  C
    MOV  A, #8
    SUBB A, R4       ; A = 8 - stage
    MOV  R6, A       ; R6 = inner loop count

    MOV  R2, #01H    ; running bit starts from right

STAGE_INNER:
    MOV  A, R2
    ORL  A, MASK     ; combine fixed left block and running bit
    ACALL SHOW_P2
    ACALL DELAY200MS

    ; shift R2 left: R2 <<= 1 (via A)
    MOV  A, R2
    RL   A
    MOV  R2, A

    DJNZ R6, STAGE_INNER

    ; compute next MASK:
    ; MASK_next = MASK OR (MASK >> 1)
    MOV  A, MASK
    MOV  TEMP, A
    CLR  C
    RRC  A           ; A = MASK >> 1
    ORL  A, TEMP     ; A = MASK OR (MASK >> 1)
    MOV  MASK, A

    INC  R4          ; stage++
    CJNE R4, #8, STAGE_OUTER

    ; final: all LEDs ON (logic 1111 1111)
    MOV  A, #0FFH
    ACALL SHOW_P2
    ACALL DELAY200MS
    RET

; 3. EFFECT_TAT_DAN
EFFECT_TAT_DAN:
    MOV  A, #0FFH    ; all LEDs ON (logic)
    MOV  R7, #9      ; 9 states: FF,7F,3F,1F,0F,07,03,01,00

FADE_OUT_LOOP:
    ACALL SHOW_P2
    ACALL DELAY200MS

    ; logical right shift with zero fill at bit7
    CLR  C
    RRC  A           ; rotate right through carry
    ANL  A, #07FH    ; ensure bit7 = 0
    DJNZ R7, FADE_OUT_LOOP
    RET

; 4. EFFECT_CHOP_TAT
EFFECT_CHOP_TAT:
    MOV  R7, #6

BLINK_LOOP:
    MOV  A, #0FFH    ; all ON (logic)
    ACALL SHOW_P2
    ACALL DELAY200MS

    MOV  A, #00H     ; all OFF (logic)
    ACALL SHOW_P2
    ACALL DELAY200MS

    DJNZ R7, BLINK_LOOP
    RET

; 5. EFFECT_SANG_DAN: right to left
EFFECT_SANG_DAN:
    MOV  A, #00H
    MOV  R7, #8

FADE_IN_LOOP:
    RL   A
    ORL  A, #01H     ; always keep bit 0 = 1, build 1-bits from right
    ACALL SHOW_P2
    ACALL DELAY200MS
    DJNZ R7, FADE_IN_LOOP
    RET
; DELAY ~200
DELAY200MS:
    MOV  R1, #200
D2_L2:
    MOV  R0, #200
D2_L1:
    NOP
    NOP
    DJNZ R0, D2_L1
    DJNZ R1, D2_L2
    RET

END