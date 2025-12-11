#include <REGX51.H>
#include <stdio.h>
#include <string.h>

sbit LED = P2^0;      

volatile bit flag = 0;           
volatile unsigned char recvData; // D? li?u nh?n du?c

void UART_Init(void) {
    SCON = 0x50;   /
    TMOD &= 0x0F;  // Xóa các bit ch? d? Timer 1
    TMOD |= 0x20;  
    TH1  = 0xFD;   // baud 9600, SMOD=0
    TR1  = 1;      // Kích hoat Timer 1
    TI   = 1;      
}
void UART_SendChar(char c) {
    while (!TI);        
    TI = 0;             
    SBUF = c;           
}
void UART_SendString(char *str) {
    while (*str) {                
        UART_SendChar(*str);       
        str++;                     
    }
}
void UART_EnableInterrupt(void) {
    ES = 1;    
    EA = 1;    
}
void UART_ISR(void) interrupt 4 {
    if (RI) {                 
        RI = 0;               
        recvData = SBUF;      
        flag = 1;             
    }
}
void main(void)
{
    LED = 1;               
    UART_Init();           
    UART_EnableInterrupt();

    while(1)
    {
        if(flag == 1)
        {
            if(recvData == '1')
                LED = 0;   
            else
                LED = 1;   

            UART_SendString("\rOK\r");
            flag = 0;
        }

    }
}
