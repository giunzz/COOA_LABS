#include <REGX51.H>

sbit LED0 = P2^0;   

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 123; j++);   
}

void main() {
    while (1) {
        LED0 = 0;        
        delay_ms(1000);   
        LED0 = 1;        
        delay_ms(1000);   
    }
}
