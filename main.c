#include <stdint.h>
#include "PLL.h"
#include "SysTick.h"
#include "tm4c123gh6pm.h"

#define PA 0x01
#define PB 0x02

#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523


//uint32_t delay = 0;
//int32_t i;
int main(){
		uint32_t LEDPattern[16] = {0xFFF,0xEEE,0xDDD,0xCCC,0xBBB,0xAAA,0x999,0x888,0x777,0x666,0x555,0x444,0x333,0x222,0x111,0x000}; 
		/* The array for the color for LED display
       23  22  21  20  19  18  17  16  15  14  13  12  11  10  9   8   7   6   5   4   3   2   1   0
			 G7  G6  G5  G4  G3  G2  G1  G0  R7  R6  R5  R4  R3  R2  R1  R0  B7  B6  B5  B4  B3  B2  B1  B0 
		*/
		
		//uint32_t delay = 0;
		int32_t i;
		
				// call the function to initialize PLL
		PLL_Init();
		
				// call the function to initialize SysTick
		SysTick_Init();
		  /* enable clock to GPIOA */
		  /* enable clock to GPIOB */
		SYSCTL_RCGCGPIO_R = PA | PB;
		 /* set all PORTB pins as output to drive LEDs */
		GPIO_PORTB_DIR_R = 0xFF;
     /* set all PORTB pins as digital pins */
		GPIO_PORTB_DEN_R =0xFF;
     /* set PORTA pin 7 as output to drive the speaker */
		GPIO_PORTA_DIR_R = 0x80;
     /* set PORTA pin 7 as a digital pin */
		GPIO_PORTA_DEN_R = 0x80;
		// The code for the countdown timer. Hint: a for loop
		for(i = 15; i >= 0 ; i--){
			GPIO_PORTB_DATA_R = LEDPattern[i];
			SysTick_Wait200ms(2);
		}
/*		
		// after the timed countdown, start the DC motor with the duty cycle to be 80%.
		while (1){
					// on for .8 ms, output through PA7
					GPIO_PORTA_DATA_R = 0x80;
					// assume 50 MHz clk, calculate the delay value for 0.8 ms 
				  // Use SysTick to create the 0.8 ms delay
					SysTick_Wait08ms(1);
					// off for .2 ms, output through PA7
					GPIO_PORTA_DATA_R = 0x00;
					// assume 50 MHz clk, calculate the delay value for 0.2 ms
					// Use SysTick to create the 0.2 ms delay
					SysTick_Wait02ms(1);
		}
*/		
}
