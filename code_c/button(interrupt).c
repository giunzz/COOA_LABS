#include <REGX51.H>

sbit LED1 = P1^0;
sbit LED2 = P1^1;

sbit BTN_INT0 = P3^2;   // INT0
sbit BTN_INT1 = P3^3;   // INT1

void int0_isr(void) interrupt 0
{
    LED1 = ~LED1;   // toggle LED1
}
void int1_isr(void) interrupt 2
{
    LED2 = ~LED2;   // toggle LED2
}
void main(void)
{
    LED1 = LED2 = 1;    // OFF

    IT0 = 1;    // INT0 edge-trigger (falling edge)
    IT1 = 1;    // INT1 edge-trigger

    EX0 = 1;    // enable INT0
    EX1 = 1;    // enable INT1
    EA  = 1;    // global interrupt enable

    while (1)
    {
        // không c?n làm gì
        // CPU r?nh
    }
}
