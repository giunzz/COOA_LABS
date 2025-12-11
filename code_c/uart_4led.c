#include <REGX51.H>
#include <stdio.h>
sbit LED1 = P2^0;
sbit LED2 = P2^1;
sbit LED3 = P2^2;
sbit LED4 = P2^3;
// Khoi tao UART, Baud 9600bps, thach anh 11.0592MHz
void UART_Init(void) {
    TMOD = 0x20;    // Timer1, mode 2 (8-bit auto reload)
    TH1  = 0xFD;    // Toc do baud 9600
    SCON = 0x50;    // UART mode 1, 8-bit, REN=1
    TR1  = 1;
       //
}
void UART_TxChar(char c) {
    SBUF = c;
    while (TI == 0);
    TI = 0;
}
void UART_SendString(char *s) {
    while (*s) {
        UART_TxChar(*s++);
    }
}
void Display_Menu(void) {
    UART_SendString("\r\n===== MENU DIEU KHIEN LED =====\r\n");
    UART_SendString("1. BAT LED 1\r\n");
    UART_SendString("2. TAT LED 1\r\n");
    UART_SendString("3. BAT LED 2\r\n");
    UART_SendString("4. TAT LED 2\r\n");
    UART_SendString("5. BAT LED 3\r\n");
    UART_SendString("6. TAT LED 3\r\n");
    UART_SendString("7. BAT LED 4\r\n");
    UART_SendString("8. TAT LED 4\r\n");
    UART_SendString("================================\r\n");
    UART_SendString("Trang thai LED (0 = sang, 1 = tat): ");
    UART_TxChar(LED1 ? '1' : '0');
    UART_TxChar(' ');
    UART_TxChar(LED2 ? '1' : '0');
    UART_TxChar(' ');
    UART_TxChar(LED3 ? '1' : '0');
    UART_TxChar(' ');
    UART_TxChar(LED4 ? '1' : '0');
    UART_SendString("\r\nNhap lenh: ");
}
void main(void) {
    char ch;
    UART_Init();
    LED1 = LED2 = LED3 = LED4 = 1; 
    Display_Menu();
    while (1) {
        while (RI == 0);  
        ch = SBUF;
        RI = 0;
        switch (ch) {
            case '1': LED1 = 0; break;  
            case '2': LED1 = 1; break; 
            case '3': LED2 = 0; break;
            case '4': LED2 = 1; break;
            case '5': LED3 = 0; break;
            case '6': LED3 = 1; break;
            case '7': LED4 = 0; break;
            case '8': LED4 = 1; break;
            default:
                UART_SendString("\r\nLenh khong hop le!\r\n");
                break;
        }
        Display_Menu(); 
    }
}