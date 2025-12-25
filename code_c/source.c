#include <REGX51.H>
#include <stdio.h>
#define SEGMENT_PORT P0  
#define SELECT_PORT  P2 


sbit LED1 = P1^7;
sbit LED2 = P1^6;
sbit LED3 = P1^5;

sbit BTN1 = P3^2;   
sbit BTN2 = P3^3; 
volatile unsigned int PERIOD  = 0, ms_tick = 0, state = 0;
void UART_Receive_Handler(void);

volatile unsigned char scan_index = 0;   

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 123; j++);
}


/* Timer 1 Uart */
void UART_Init(void) {
    SCON = 0x50;        // UART mode 1, enable receiver
    TMOD &= 0x0F;       // Clear Timer1 config
    TMOD |= 0x20;       // Timer1 mode 2
    TH1 = 0xFD;         // Baud 9600 (11.0592 MHz)
    TR1 = 1;            // Start Timer1
    TI = 1;             // Allow first transmit
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


unsigned char digit_patterns[] = {
    0x3F,   // 0
    0x06,   // 1
    0x5B,   // 2
    0x4F,   // 3
    0x66,   // 4
    0x6D,   // 5F
    0x7D,   // 6
    0x07,   // 7
    0x7F,   // 8
    0x6F    // 9
};
void Display_Menu(void) {
    
		UART_SendString("\r\n=====DIEU KHIEN CHU KY LED=====\r\n");
}

unsigned char uart_index = 0;
char cmd[100];

void Process_CK(void)
{
		if (cmd[0] >='0' && cmd[0] <= '7')
		{
			//UART_SendChar(cmd[2]);
			if (cmd[2] == 'n') 
			{
				//UART_SendChar(cmd[5]);
				LED1 = 0; 
				if(cmd[5] == 'n') LED2 = 0;
				else LED2 = 1;
			}
			else 
			{
				//UART_SendChar(cmd[6]);
				LED1 = 1;
				if (cmd[6] == 'n') LED2 = 0;
				else LED2 = 1;
			}
			if (LED1 == 0 && LED2 == 0) LED3 = 0;
			else LED3 = 1;		
		}
		if (cmd[0] == 'M' && cmd[1] == '1')
		{
			PERIOD =(cmd[2]-'0') * 1000 + (cmd[3]-'0')*100 + (cmd[4]-'0') * 10 + (cmd[5]-'0');
			if (LED1 == 0) delay_ms(PERIOD);
			LED1 = 1;
			if (LED2 == 0) delay_ms(PERIOD);
			LED2 = 1;
			if (LED3 == 0) delay_ms(PERIOD);
			LED3 = 1;
		}
		if (cmd[0] == 'M' && cmd[1] == '0')
		{
			LED1 = LED2 = LED3 = 1;
			if (state == 1) 
			{
				IT0 = 1;   
				IT1 = 1;  
				EX0 = 1;   
				EX1 = 1;   
				EA  = 1;
			}
			else 
			{
				IT0 = 0;   
				IT1 = 0;  
				EX0 = 0;   
				EX1 = 0;   
				EA  = 0;
			}
		}
		UART_SendString("\r\nSet OK\r\n");
}

void UART_Receive_Handler(void)
{
    char c;
		
    if (RI)
    {
        RI = 0;
        c = SBUF;
				
				if (c == '\r' || c == '\n')
				{
					 Process_CK();  
           uart_index = 0; 
					 if (cmd[0] == 'M' && cmd[1] == '0') state = ~state;
				}
				else 
				{
					cmd[uart_index] = c;   
					uart_index++;					
        }
			}
}
        


void display_digit(unsigned char index, unsigned char value, unsigned char dot)
{
		if (dot == 0){
			SELECT_PORT = (index << 2);
			SEGMENT_PORT = digit_patterns[value];
			delay_ms(1);
		}
		else 
		{
			SELECT_PORT = (index << 2);
			SEGMENT_PORT = digit_patterns[value] | 0x80;
			delay_ms(1);
		}
			
}

void int0_isr(void) interrupt 0
{
		LED1 = ~LED1; 
		if (LED1 == 0 && LED2 == 0) LED3 = 0;
		else LED3 = 1;
}
void int1_isr(void) interrupt 2
{	
    LED2 = ~LED2; 
		if (LED1 == 0 && LED2 == 0) LED3 = 0;
		else LED3 = 1;
}


void main(void)
{
		UART_Init();
		Display_Menu();
		state = 0;
    LED1 = LED2 = LED3 = 1;
    IT0 = 1;   
    IT1 = 1;  
    EX0 = 1;   
    EX1 = 1;   
    EA  = 1;  
    while (1) {
			UART_Receive_Handler();
			if (LED1 == 1 && LED2 == 1 && LED3 == 1)
			{			
				display_digit(0, ~LED1, 0); 
				display_digit(1, ~LED2, 0); 
				display_digit(2, ~LED3, 0); 
				display_digit(3, 0, 0); 
			}
			else 
			{
				display_digit(0, ~LED1, 0); 
				display_digit(1, ~LED2, 0); 
				display_digit(2, ~LED3, 0); 
				display_digit(3, 1, 0); 
			}
    }
}
