#include <REGX51.H>   
#define SEGMENT_PORT P0   // Điều khiển các đoạn LED
#define SELECT_PORT  P2   // Chọn LED 7 đoạn

// Bảng mã LED 7 đoạn (Cathode chung)
unsigned char digit_patterns[] = {
    0x3F,   // 0
    0x06,   // 1
    0x5B,   // 2
    0x4F,   // 3
    0x66,   // 4
    0x6D,   // 5
    0x7D,   // 6
    0x07,   // 7
    0x7F,   // 8
    0x6F    // 9
};
unsigned char led7seg = 0;
unsigned char value  = 0;

void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 123; j++);   // delay khoảng 1ms
}
void main(void)
{
    SEGMENT_PORT = 0x00;  // Tắt tất cả đoạn LED ban đầu

    while (1)
    {
        SELECT_PORT = ((led7seg & 0x07) << 2);

        SEGMENT_PORT = digit_patterns[value & 0x07];

        delay_ms(2);        
        led7seg++;          
        led7seg &= 0x07;    

        value++;
        value &= 0x07;      
    }
}
