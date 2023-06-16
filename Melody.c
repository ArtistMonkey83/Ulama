#include <stdint.h>
#include "SysTick.h"
#include "tm4c123gh6pm.h"


/* Function for playing a tone when Ulama is activated*/
void SpeakerOn(){
// on for .8 ms, output through PC7
					GPIO_PORTC_DATA_R = 0x80;
					// assume 50 MHz clk, calculate the delay value for 0.8 ms 
				  // Use SysTick to create the 0.8 ms delay
					SysTick_Wait08ms(1);
					// off for .2 ms, output through PC7
					GPIO_PORTC_DATA_R = 0x00;
					// assume 50 MHz clk, calculate the delay value for 0.2 ms
					// Use SysTick to create the 0.2 ms delay
					SysTick_Wait02ms(1);
}		

/*Function for playing a tone when Ulama is shut down */
void SpeakerOff(){
					GPIO_PORTC_DATA_R = 0x80;
					// assume 50 MHz clk, calculate the delay value for 1.6 ms 
				  // Use SysTick to create the 1.6 ms delay
					SysTick_Wait08ms(2);
					// off for 0.8 ms, output through PC7
					GPIO_PORTC_DATA_R = 0x00;
					// assume 50 MHz clk, calculate the delay value for 0.8 ms
					// Use SysTick to create the 0.8 ms delay
					SysTick_Wait02ms(3);
}	


/*Function for playing a tone when a goal is made */
void SpeakerGoal(){
					GPIO_PORTC_DATA_R = 0x80;
					// assume 50 MHz clk, calculate the delay value for 1.6 ms 
				  // Use SysTick to create the 1.6 ms delay
					SysTick_Wait08ms(2);
					// off for 0.8 ms, output through PC7
					GPIO_PORTC_DATA_R = 0x00;
					// assume 50 MHz clk, calculate the delay value for 0.8 ms
					// Use SysTick to create the 0.8 ms delay
					SysTick_Wait02ms(3);
}
