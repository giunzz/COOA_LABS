#include <REGX51.H>
#include <stdio.h>
#define SEGMENT_PORT P0  
#define SELECT_PORT  P2 
#define NUM_7SEG 4

sbit LED1 = P1^0;
sbit LED2 = P1^7;
sbit LED3 = P1^5;
sbit LED4 = P1^4;

volatile unsigned int tick_10ms = 0, tick_1ms = 0;
volatile unsigned int cnt_led1 = 0, cnt_7seg = 0;
volatile unsigned int cnt_led2 = 0;

/* Button polling */
sbit BTN3 = P3^4;
sbit BTN4 = P3^5;
/* Button interrupt */
sbit BTN1 = P3^2;   
sbit BTN2 = P3^3; 

/* Chu ky tính theo tick 10ms */
volatile unsigned int LED1_PERIOD  = 200; // 2s  = 200 x 10ms
volatile unsigned int LED2_PERIOD  = 100;// 1s  = 100 x 10ms

volatile unsigned char scan_index = 0;   
char cmd[5];
unsigned char uart_index = 0;

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
    UART_SendString("\r\n===== TRANG THAI =====\r\n");
    UART_SendString(!LED1 ? "LED1 ON\r\n" : "LED1 OFF\r\n");
    UART_SendString(!LED2 ? "LED2 ON\r\n" : "LED2 OFF\r\n");
    UART_SendString(!LED3 ? "LED3 ON\r\n" : "LED3 OFF\r\n");
    UART_SendString(!LED4 ? "LED4 ON\r\n" : "LED4 OFF\r\n");
    UART_SendString("=====================\r\n");
		UART_SendString("\r\n=====DIEU KHIEN CHU KY LED=====\r\n");
    UART_SendString("nhap theo format L<led_id>:<seconds>.<decimal><ENTER> ms \r\n");
}
void Process_Command(void)
{
    Display_Menu();
		UART_SendString("\r\nSet OK\r\n");
}
/*
volatile unsigned int input_value = 0;
void UART_Receive_Handler(void)
{
    char c;
    if (RI)
    {
        RI = 0;
        c = SBUF;

        if (c >= '0' && c <= '9')
        {
            input_value = input_value * 10 + (c - '0');
        }
        else if (c == '\r' || c == '\n')
        {
            if (input_value > 0)
            {
                LED1_PERIOD = input_value;   // ms
                UART_SendString("\r\nSet OK\r\n");
            }
            input_value = 0;   
        }
    }
}*/

void UART_SendNumber(unsigned int num)
{
    char buf[6];
    char i = 0;

    if (num == 0)
    {
        UART_SendChar('0');
        return;
    }

    while (num > 0)
    {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    }

    while (i--)
    {
        UART_SendChar(buf[i]);
    }
}

volatile unsigned char led_select = 0;   // 1, 2, ...
volatile unsigned int int_part = 0;       // ph?n nguyên (giây)
volatile unsigned char frac_part = 0;     // 1 ch? s? th?p phân
volatile bit dot_seen = 0;
volatile bit parse_started = 0;
void UART_Receive_Handler(void)
{
    char c;

    if (RI)
    {
        RI = 0;
        c = SBUF;


        if (c == 'L')
        {
            led_select = 0;
            int_part = 0;
            frac_part = 0;
            dot_seen = 0;
            parse_started = 1;
        }
        else if (parse_started && led_select == 0 && c >= '1' && c <= '4')
        {
            led_select = c - '0';
        }
        /* D?u ':' */
        else if (parse_started && c == ':')
        {
            
        }
        else if (parse_started && c >= '0' && c <= '9')
        {
            if (!dot_seen)
            {
                int_part = int_part * 10 + (c - '0');
            }
            else
            {
                frac_part = (c - '0');   
            }
        }
        else if (parse_started && c == '.')
        {
            dot_seen = 1;
        }
        else if (c == '\r' || c == '\n')
        {
            unsigned int period_ms;

            period_ms = int_part * 1000 + frac_part * 100;

            if (led_select == 1)
            {
                LED1_PERIOD = period_ms/10;
								UART_SendNumber(LED1_PERIOD);
            }
            else if (led_select == 2)
            {
                LED2_PERIOD = period_ms/10;
								UART_SendNumber(LED2_PERIOD);
            }

            UART_SendString("\r\nSet OK\r\n");

            /* reset */
            parse_started = 0;
            led_select = 0;
            int_part = 0;
            frac_part = 0;
            dot_seen = 0;
        }
    }
}

/* timer 0 cho led 10ms */
void Timer0_ISR(void) interrupt 1
{
    TH0 = 0xDC;
    TL0 = 0x00;

    tick_10ms++;

    cnt_led1++;
    if (cnt_led1 == LED1_PERIOD/2)
		{
        LED1 = ~LED1;     
				cnt_led1 = 0;
		}
    
    cnt_led2++;
    if (cnt_led2 == LED2_PERIOD/2)
		{
        LED2 = ~LED2;          
				cnt_led2 = 0;
    }
}

void Timer0_Init(void)
{
    TMOD &= 0xF0;
    TMOD |= 0x01;         

    TH0 = 0xDC;
    TL0 = 0x00;

    ET0 = 1;               
    TR0 = 1;               
}
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 123; j++);
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
void scan_buttons(void)
{
    static bit last_b3 = 1, last_b4 = 1;
    
		if (BTN3 == 0 && last_b3 == 1) {
        if (LED1_PERIOD > 100)
            LED1_PERIOD -= 50;
				else LED1_PERIOD  = 500;
				Process_Command();
    }
    last_b3 = BTN3;

    if (BTN4 == 0 && last_b4 == 1) {
        if (LED2_PERIOD > 100)
            LED2_PERIOD -= 50;
				else LED2_PERIOD  = 500;
				Process_Command();
    }
    last_b4 = BTN4;
}

void int0_isr(void) interrupt 0
{
		LED1_PERIOD += 50;
		if (LED1_PERIOD >= 600) LED1_PERIOD = 100;
    LED3 = ~LED3;  
		Process_Command();
}
void int1_isr(void) interrupt 2
{	
    LED4 = ~LED4;
		LED2_PERIOD += 50;
		if (LED2_PERIOD >= 600) LED2_PERIOD = 100;
		Process_Command();
}


void main(void)
{
		UART_Init();
		Display_Menu();
	
    LED1 = LED2 = LED3 = LED4 = 1;
    IT0 = 1;   
    IT1 = 1;  
    EX0 = 1;   
    EX1 = 1;   
    EA  = 1;  
    
		Timer0_Init();
		
	
    while (1) {
			scan_buttons(); 
			UART_Receive_Handler();
			display_digit(7, LED1_PERIOD/100, 1); 
			display_digit(6, (LED1_PERIOD%100)/10, 0); 
			display_digit(1, LED2_PERIOD/100, 1); 
			display_digit(0, LED2_PERIOD%100/10, 0); 
    }
}
