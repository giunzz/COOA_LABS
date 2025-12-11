#include <REGX51.H>

void delay_ms(unsigned int ms) {
    unsigned int i, j;
	  for (i = 0; i < ms; i++)
       for (j = 0; j < 123; j++);
}

void main() {
    while (1) {

        P2 = 0xFE;   // 1111 1110 - LED bit 0 sáng
        delay_ms(200);

        P2 = 0xFD;   // 1111 1101 - LED bit 1 sáng
        delay_ms(200);

        P2 = 0xFB;   // 1111 1011 - LED bit 2 sáng
        delay_ms(200);

        P2 = 0xF7;   // 1111 0111 - LED bit 3 sáng
        delay_ms(200);

        P2 = 0xEF;   // 1110 1111 - LED bit 4 sáng
        delay_ms(200);

        P2 = 0xDF;   // 1101 1111 - LED bit 5 sáng
        delay_ms(200);

        P2 = 0xBF;   // 1011 1111 - LED bit 6 sáng
        delay_ms(200);

        P2 = 0x7F;   // 0111 1111 - LED bit 7 sáng
        delay_ms(200);
    }
}
