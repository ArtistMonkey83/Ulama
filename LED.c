#include <stdint.h>
#include "SysTick.h"
#include "tm4c123gh6pm.h"

/* The array for the color for LED display
		
       23  22  21  20  19  18  17  16  15  14  13  12  11  10  9   8   7   6   5   4   3   2   1   0
			 G7  G6  G5  G4  G3  G2  G1  G0  R7  R6  R5  R4  R3  R2  R1  R0  B7  B6  B5  B4  B3  B2  B1  B0 
*/

//uint32_t LEDPattern[16] = {0xFFF,0xEEE,0xDDD,0xCCC,0xBBB,0xAAA,0x999,0x888,0x777,0x666,0x555,0x444,0x333,0x222,0x111,0x000}; 

/* The LEDs should flash 3 times and then stay on when Ulama is activated */
void LEDon(){
		/* Pin1 == On/Off, Red LEDs flash 3 times and then stay on when On/Off switch flipped*/
			GPIO_PORTB_DATA_R = 0x00;		//Turn off the red LEDs
			GPIO_PORTB_DATA_R = 0x02;		//Turn on the red LEDs
			SysTick_Wait200ms(1);				// Wait
			GPIO_PORTB_DATA_R = 0x00;		//Turn off the red LEDs
			SysTick_Wait200ms(1);				// Wait
			GPIO_PORTB_DATA_R = 0x02;		//Turn on the red LEDs
			SysTick_Wait200ms(1);				// Wait
			GPIO_PORTB_DATA_R = 0x00;		//Turn off the red LEDs
			SysTick_Wait200ms(1);				// Wait
			GPIO_PORTB_DATA_R = 0x02;		//Turn on the red LEDs
}

/* The ON/OFF LEDs should flash faster 2 times, (DIM), and then stay off when Ulama is deactivated */
void LEDoff(){
			GPIO_PORTB_DATA_R = 0x00;		//Turn off the red LEDs
			SysTick_Wait200ms(3);				// Wait
			GPIO_PORTB_DATA_R = 0x02;		//Turn on the red LEDs
			SysTick_Wait200ms(3);				// Wait
			GPIO_PORTB_DATA_R = 0x00;		//Turn off the red LEDs
}

void LEDgoal(){
			GPIO_PORTB_DATA_R = 0x00;		//Turn off the red LEDs
			GPIO_PORTB_DATA_R = 0x02;		//Turn on the red LEDs
			SysTick_Wait200ms(1);				// Wait
			GPIO_PORTB_DATA_R = 0x00;		//Turn off the red LEDs
			SysTick_Wait200ms(1);				// Wait
			GPIO_PORTB_DATA_R = 0x02;		//Turn on the red LEDs
			SysTick_Wait200ms(1);				// Wait
			GPIO_PORTB_DATA_R = 0x00;		//Turn off the red LEDs
			SysTick_Wait200ms(1);				// Wait
			GPIO_PORTB_DATA_R = 0x02;		//Turn on the red LEDs
}

//void LEDindoor(){}
//void LEDoutdoor(){}
