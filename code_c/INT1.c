#include <REGX51.H>

sbit LED = P2^0;
unsigned char count = 0;

 // INT1 (P3.3) bat
void external1_isr(void) interrupt 2 {
    if (P3^3 == 0) { 
        count++;
        P2 = count;
        while (P3^3 == 0);

    }
}

void main(void) {
    IT1 = 1;   
    EX1 = 1;   
    EA  = 1;  
    P2 = 0;
    while (1) {
    }
}
