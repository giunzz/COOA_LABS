#include <iostream>

void delay200ms() {
    unsigned char R2, R1;
    for(R2 = 200; R2 > 0; R2--) {
        for(R1 = 200; R1 > 0; R1--) {
        }
    }
}

void delay1s() {
    unsigned char R3;
    for(R3 = 5; R3 > 0; R3--) {
        delay200ms();
    }
}

void main()
{
    unsigned char base = 0xFF;   // 1111 1111 (tất cả led tắt)
    unsigned char mask;
    unsigned char i, j;

    while(1)
    {
        // 8 lần (mỗi lần giảm bớt 1 LED sáng cố định)
        for(i = 0; i < 8; i++)
        {
            mask = base << i;   // tạo vùng LED luôn sáng

            // sáng dần các LED còn lại (từ P2.0 → P2.(7-i))
            for(j = 0; j < (8 - i); j++)
            {
                P2 = mask & ~(1 << j);   // sáng từng LED một
                delay200ms();
            }
        }
    }
}