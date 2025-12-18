#include <REGX51.H>

/* LED */
sbit LED1 = P1^7;
sbit LED2 = P1^6;
sbit LED3 = P1^5;
sbit LED4 = P1^4;
sbit LED5 = P1^3;
sbit LED6 = P1^2;

/* Button polling */
sbit BTN1 = P3^0;
sbit BTN2 = P3^1;
sbit BTN5 = P3^4;
sbit BTN6 = P3^5;

/* Button interrupt */
sbit BTN3 = P3^2;   
sbit BTN4 = P3^3;   

void int0_isr(void) interrupt 0
{
    LED3 = ~LED3;   
}
void int1_isr(void) interrupt 2
{
    LED4 = ~LED4;   
}
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 123; j++);
}

void scan_buttons(void)
{
    if (BTN1 == 0) { delay_ms(20); if (BTN1 == 0) { LED1 = ~LED1; while (BTN1 == 0); } }
    if (BTN2 == 0) { delay_ms(20); if (BTN2 == 0) { LED2 = ~LED2; while (BTN2 == 0); } }
    if (BTN5 == 0) { delay_ms(20); if (BTN5 == 0) { LED5 = ~LED5; while (BTN5 == 0); } }
    if (BTN6 == 0) { delay_ms(20); if (BTN6 == 0) { LED6 = ~LED6; while (BTN6 == 0); } }
}
void main(void)
{
    LED1 = LED2 = LED3 = LED4 = LED5 = LED6 = 1;

    IT0 = 1;   
    IT1 = 1;  
    EX0 = 1;   
    EX1 = 1;   
    EA  = 1;  

    while (1)
    {
        scan_buttons();   
    }
}
