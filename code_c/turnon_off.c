#include<REGX51.H>

void delay(unsigned int time) {
		unsigned int i , j;
		for (i = 0 ; i < time; i++)
				for (j = 0 ; j < 123; j++);
}
void main(){
	while(1){
		P1 = 0x00; // n=bat led 
		delay(500);
		P1 = 0xFF; // tat led
		delay(500);
	}
}
	