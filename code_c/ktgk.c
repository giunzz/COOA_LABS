#include <REGX51.H>

sbit BTN_DEC = P3^0;   // gi?m T
sbit BTN_INC = P3^3;   // tang T
sbit BTN_EXIT = P3^1;  // nút thoát STOP
sbit FLAG_STOP = P3^2; // dùng bi?n c?c b? thay ASM BIT 20H (INT0 s? x? lý)

unsigned char T_DELAY = 100;   // 100 × 10ms = 1s
unsigned char pattern = 0;

bit stop_flag = 0;

//--------------------------------------------------
// NGAT INT0 – d?ng chuong trình
//--------------------------------------------------
void external0_isr(void) interrupt 0 {
    stop_flag = 1;
}

//--------------------------------------------------
// Delay 10ms
//--------------------------------------------------
void delay_10ms(void) {
    TMOD &= 0xF0;      // clear Timer0 bits
    TMOD |= 0x01;      // Mode 1 (16 bit)

    TH0 = 0xD8;
    TL0 = 0xF0;

    TR0 = 1;
    while (TF0 == 0);
    TR0 = 0;
    TF0 = 0;
}

//--------------------------------------------------
// Delay theo s? l?n (T_DELAY × 10ms)
//--------------------------------------------------
void delay_variable(void) {
    unsigned char i;
    for(i = 0; i < T_DELAY; i++) {
        delay_10ms();
    }
}

//--------------------------------------------------
// C?p nh?t pattern LED
//--------------------------------------------------
void update_pattern(void) {
    pattern = (pattern << 1) | 0x01;
    if(pattern > 0x1F) {
        pattern = 0;
    }
}

//--------------------------------------------------
// STOP MODE
//--------------------------------------------------
void stop_mode(void) {
    P2 = 0x00;
    P2 |= 0xF0;      // gi? 1 các bit cao

    while(BTN_EXIT == 1);  // ch? nh?n P3.1 d? thoát STOP
    stop_flag = 0;
}

//--------------------------------------------------
// MAIN
//--------------------------------------------------
void main(void) {

    IT0 = 1;   // INT0 c?nh xu?ng
    EX0 = 1;   // cho phép INT0
    EA  = 1;   // b?t ng?t toàn c?c

    BTN_DEC = 1;   // input
    BTN_INC = 1;
    BTN_EXIT = 1;

    while(1) {

        // N?u dang STOP ? x? lý STOP MODE
        if(stop_flag) {
            stop_mode();
        }

        // X? lý nút gi?m
        if(BTN_DEC == 0) {
            if(T_DELAY > 50) T_DELAY -= 50;
            else T_DELAY = 50;
            while(BTN_DEC == 0); // ch?ng d?i
        }

        // X? lý nút tang
        if(BTN_INC == 0) {
            if(T_DELAY < 200) T_DELAY += 50;
            else T_DELAY = 200;
            while(BTN_INC == 0); // ch?ng d?i
        }

        // Xu?t pattern ra LED
        P2 = pattern;
        P2 |= 0xF0;

        // Delay theo T_DELAY
        delay_variable();

        // C?p nh?t d?ng ch?y
        update_pattern();
    }
}
