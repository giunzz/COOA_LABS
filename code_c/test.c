#include <REGX51.H>

#define SEGMENT_PORT P0  
#define SELECT_PORT  P2  
sbit LED1 = P1^0;
sbit LED2 = P1^7;

volatile unsigned int period1 = 1000;   // LED1 = 1s on/off
volatile unsigned int period2 = 500;    // LED2 = 0.5s on/off
volatile unsigned int c1 = 0, c2 = 0;

char cmd[3];  // buffer nhận lệnh UART 3 ký tự

// UART ===============================
void UART_Init(void) {
    TMOD |= 0x20;  
    TH1  = 0xFD;   
    SCON = 0x50;
    TR1  = 1;
}

void UART_TxChar(char c) {
    SBUF = c;
    while (TI == 0);
    TI = 0;
}

void UART_SendString(char *s) {
    while (*s) UART_TxChar(*s++);
}

void UART_Read3(void)
{
    char i;
    for(i = 0; i < 3; i++)
    {
        while(RI == 0);
        cmd[i] = SBUF;
        RI = 0;
        UART_TxChar(cmd[i]); // echo
    }
}

// =====================================

void small_delay()
{
    unsigned int i;
    for(i = 0; i < 200; i++);
}

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 123; j++);
}

void timer0_isr(void) interrupt 1
{
    TH0 = 0xFC;
    TL0 = 0x66;   // 1ms tick
    c1++;
    c2++;
}

void timer0_init()
{
    TMOD |= 0x01;
    TH0 = 0xFC; TL0 = 0x66;
    ET0 = 1;
    TR0 = 1;
}

unsigned char digit_patterns[] = {
    0x3F,0x06,0x5B,0x4F,0x66,
    0x6D,0x7D,0x07,0x7F,0x6F
};

void display_2digit(unsigned char d1, unsigned char d2)
{
    SELECT_PORT = (0 << 2);
    SEGMENT_PORT = digit_patterns[d1];
    delay_ms(1);

    SELECT_PORT = (7 << 2);
    SEGMENT_PORT = digit_patterns[d2];
    delay_ms(1);
}

// ========== XỬ LÝ LỆNH UART ==============
void Process_Command()
{
    unsigned char led = cmd[0] - '0';
    unsigned int time = (cmd[1] - '0')*10 + (cmd[2] - '0');
    unsigned int new_period = time * 100;  // đổi ra ms

    // Giới hạn 500 → 2500 ms
    if(new_period < 500) new_period = 500;
    if(new_period > 2500) new_period = 2500;

    switch(led)
    {
        case 1:
            period1 = new_period;
            UART_SendString("\r\nDa thay doi LED1\r\n");
            break;
        case 2:
            period2 = new_period;
            UART_SendString("\r\nDa thay doi LED2\r\n");
            break;
        default:
            UART_SendString("\r\nLenh khong hop le!\r\n");
            return;
    }

    UART_SendString("Chu ky moi = ");
    UART_TxChar(cmd[1]);
    UART_TxChar(cmd[2]);
    UART_SendString(" (x0.1s)\r\n");
}

// =========================================

void main()
{
    LED1 = LED2 = 1;

    UART_Init();
    timer0_init();

    UART_SendString("\r\n=== DIEU KHIEN LED QUA UART ===\r\n");
    UART_SendString("Nhap lenh (VD: 105, 210)\r\n");

    EA = 1;

    while(1)
    {
        UART_SendString("\r\nNhap lenh: ");
        UART_Read3();
        Process_Command();

        // Blink LED1
        if(c1 >= period1)
        {
            LED1 = !LED1;
            c1 = 0;
        }

        // Blink LED2
        if(c2 >= period2)
        {
            LED2 = !LED2;
            c2 = 0;
        }

        // Hiển thị chu kỳ dạng số 1 → 5
        display_2digit(period1/500, period2/500);
    }
}
