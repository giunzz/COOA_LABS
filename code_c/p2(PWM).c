#include <REGX51.H>
#include <intrins.h>
sbit LED = P2^2;  
 void delay_us(unsigned int us) {
     while (us--) {
         _nop_();      // mất 1µs nếu chạy ở 12MHz
     }
 }

 void main() {
     unsigned char i, duty_cycle;
     while (1) {

         // Tăng dần độ sáng
         for (duty_cycle = 0; duty_cycle < 100; duty_cycle++) {

             for (i = 0; i < 100; i++) {
                 if (i < duty_cycle)
                     LED = 0;   // Bật LED (active low)
                 else
                     LED = 1;   // Tắt LED
             }
             delay_us(10);   // Điều chỉnh độ dài xung PWM (~1ms chung kỳ)
         }

         // Giảm dần độ sáng
         for (duty_cycle = 100; duty_cycle > 0; duty_cycle--) {

             for (i = 0; i < 100; i++) {
                 if (i < duty_cycle)
                     LED = 0;
                 else
                     LED = 1;
             }
             delay_us(10);
         }
     }
 }
