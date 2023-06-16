#include <stdint.h>
#include "SysTick.h"
#include "tm4c123gh6pm.h"


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

/* The LEDs should flash faster 2 times, (DIM), and then stay off when Ulama is deactivated */
void LEDoff(){
			GPIO_PORTB_DATA_R = 0x00;		//Turn off the red LEDs
			SysTick_Wait200ms(3);				// Wait
			GPIO_PORTB_DATA_R = 0x02;		//Turn on the red LEDs
			SysTick_Wait200ms(3);				// Wait
			GPIO_PORTB_DATA_R = 0x00;		//Turn off the red LEDs
}

void LEDgoal(){

}

//void LEDindoor(){}
//void LEDoutdoor(){}