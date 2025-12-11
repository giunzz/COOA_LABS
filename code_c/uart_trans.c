#include <REGX51.H>
#include <string.h>

// (fosc = 11.0592 MHz)
void UART_Init(void) {
    SCON = 0x50;   // Mode 1, 8-bit, REN=1 (cho phép nh?n)
    TMOD &= 0x0F;  // Xóa các bit ch? d? Timer 1
    TMOD |= 0x20;  // Thi?t l?p Timer 1 ? mode 2 (auto reload)
    TH1 = 0xFD;   
    TR1 = 1;       // Kích ho?t Timer 1
    TI = 1;        
}
void UART_SendChar(char c) {
    while (!TI);   
    TI = 0;        
    SBUF = c;      
}
char UART_GetChar(void) {
    while (!RI);   
    RI = 0;        
    return SBUF;   
}
void UART_SendString(char *str) {
    while (*str) {
        UART_SendChar(*str);  
        str++;                
    }
}
void main(void) {
    char received_char;

    UART_Init();

    UART_SendString("8051 UART Echo Demo\r\n");
    UART_SendString("Type 1 character and Enter:\r\n");

    while (1) {
        received_char = UART_GetChar();

        UART_SendString("\rReceived Character: ");

        UART_SendChar(received_char);
        UART_SendChar('\r');
    }
}
