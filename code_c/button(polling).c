#include <REGX51.H>

sbit LED     = P2^7;
sbit BTN_ON  = P3^0;
sbit BTN_OFF = P3^1;

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 123; j++);   // delay 1ms
}

void main() {
    LED = 1;       
    BTN_ON  = 1;   
    BTN_OFF = 1;   
    while (1) {
        if (BTN_ON == 0) {      
            delay_ms(20);       
            if (BTN_ON == 0) {
                LED = 0;        
                while (BTN_ON == 0);   
            }
        }

        if (BTN_OFF == 0) {     
            delay_ms(20);       
            if (BTN_OFF == 0) {
                LED = 1;         
                while (BTN_OFF == 0);  
            }
        }
    }
}
