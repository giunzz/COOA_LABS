#include <iostream>
#include <bitset>
using namespace std;
#define P2 (*(volatile unsigned char*)0xA0)

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

void process()
{
    unsigned char mask = 0xFF;   // 1111 1111 (tất cả led tắt)
    for (int i = 0 ; i < 8; i++) {
        mask = mask << 1; 
        cout << i << ": " << " hex=0x" << hex << uppercase << (int)mask << " bin=" << bitset<8>(mask) << endl;
        for (int j = i; j < 8; j++) {
            cout << "    " << j << ": " << " hex=0x" << hex << uppercase << (int)(mask & ~(1 << j)) << " bin=" << bitset<8>(mask & ~(1 << j)) << endl;
            delay200ms();
        }
    }
    
}
int main()
{
        for (int z = 0; z < 2; z++)
        {   
            process();
        }
}