#include <REGX51.H>

void delay200ms() {
    unsigned char R2, R1;
    for(R2 = 100; R2 > 0; R2--) {
        for(R1 = 100; R1 > 0; R1--) {
        }
    }
}

void main()
{
    while(1)
    {
				unsigned char base = 0xFF;   
				unsigned char i, j, mask = 0xFF;
        for(i = 0; i < 8; i++)
        {
            base = base >> 1;   
            for(j = 0; j < 8-i; j++)
            {
                P2 = mask & ~(1 << j);   
                delay200ms();
            }
						mask = base;
        }
    }
}
