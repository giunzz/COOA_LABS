#include <REGX51.H>

sbit LED = P2^7;

// INT0 (P3.2) tat
 void external0_isr(void) interrupt 0 {
	 LED = 1;
 }

 // INT1 (P3.3) bat
 void external1_isr(void) interrupt 2 {
	 LED = 0;
 }

 void main(void) {
     IT0 = 1;   
     IT1 = 1;   
     EX0 = 1;   
     EX1 = 1;   
     EA  = 1;  
		 
	   LED = 1;
		 P3_2 = 1;
	   P3_3 = 1;
     while (1) {
     }
 }
