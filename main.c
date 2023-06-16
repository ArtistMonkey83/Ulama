#include <stdint.h>
#include "PLL.h"
#include "SysTick.h"
#include "Melody.h"
#include "LED.h"
#include "tm4c123gh6pm.h"

// The Layout of Ports on the Tiva TM4C123G
//		b31 ... b6 || b5  b4  b3  b2  b1  b0
//     RESERVED	 || F   E   D   C   B   A
//         0x...    20  10  08  04  02  01

#define PA 0x01		//SPI: Pin2 SCK[0], Pin3 CS[0], Pin4 MISO[0], Pin5 MOSI[0] I2C: Pin6 SCL[1], Pin7 SDA[1]		
#define PB 0x02		//HW Serial: Pin0 Rx[1], Pin1 Tx[1] I2C: Pin2 SCL[0], Pin3 SDA[0] SPI: Pin4 SCK[2], Pin5 CS[2], Pin6 MISO[2], Pin7 MOSI[2]		
#define PC 0x04		//HW Serial: Pin4 Rx[1], Pin5 Tx[1] Pin6 Rx[3], Pin7 Tx[3]		
#define PD 0x08		//I2C: Pin0 SCL[3], Pin1 SDA[3] SPI: Pin0 SCK[3], Pin1 CS[3], Pin2 MISO[3], Pin3 MOSI[3] HW Serial: Pin6 Rx[2], Pin7 Tx[2]		
#define PE 0x10		//HW Serial: Pin0 Rx[7], Pin1 Tx[7] Analog: Pin2 & Pin3 I2C: Pin4 SCL[2], Pin5 SDA[2]
#define PF 0x20		//SPI: Pin0 MISO[1], Pin1 MOSI[1], Pin2 SCK[1], Pin3 CS[1]


//uint32_t delay = 0;
//int32_t i;
int main(){
		/* The array for the color for LED display
		
       23  22  21  20  19  18  17  16  15  14  13  12  11  10  9   8   7   6   5   4   3   2   1   0
			 G7  G6  G5  G4  G3  G2  G1  G0  R7  R6  R5  R4  R3  R2  R1  R0  B7  B6  B5  B4  B3  B2  B1  B0 
		*/
		//uint32_t LEDPattern[16] = {0xFFF,0xEEE,0xDDD,0xCCC,0xBBB,0xAAA,0x999,0x888,0x777,0x666,0x555,0x444,0x333,0x222,0x111,0x000}; 
		
		//uint32_t delay = 0;
		int32_t PwrDwn,	//Variable holding state of the game, is it on==0 or off==1?
						goal;		//Variable holding state of goals, was one made==1 or no goal==0?
/*Polling if the game is turned on*/
		PwrDwn = 0;// Remove this when implementing Polling
	
		// call the function to initialize PLL
		PLL_Init();
		
		// call the function to initialize SysTick
		SysTick_Init();
/* enable clock to GPIOA */
		/* enable clock to GPIO B and C */
		SYSCTL_RCGCGPIO_R = /*PA | */ PB | PC;
		/* set PORTB pins 0 & 1 as output to drive LEDs */
		GPIO_PORTB_DIR_R = 0x03;
		/* set PORTB pins 0 & 1 as digital pins */
		GPIO_PORTB_DEN_R = 0x03;
		
		/* set PORTC pin 4 as input (turning on switch) and 7 as output (speaker)*/
		GPIO_PORTC_DIR_R = 0x80;
		/* set PORTC pin 4 & 7 as digital pins */
		GPIO_PORTC_DEN_R = 0x90;
		
		//while(1){
			PwrDwn = 0; // Is there incomming signal from on/off button?
			/* If the PwrDwn variable is == 0 the game is turned on */		
			if(PwrDwn == 0){
				LEDon(); //Turn on the LEDs
		/* Display Welcome Message on screen */
		/* Play boot up tones on speaker */	
				SpeakerOn(); //Play the activation tone
		/* Check if there is a goal scored */
				if(goal == 1){
					LEDgoal();
					SpeakerGoal();
				}
		}
			/* If the game is swtiched off  */		
			else if (PwrDwn == 1){
/* Call Shutdown function */
				LEDoff();	//Turn off the LEDs when powering down
				SpeakerOff();	//play tone when powering down
		/* Display Welcome Message on screen */
		/* Play boot up tones on speaker */
		}

		//}
/* Pin0 == Goal, Green LEDs flash when button pushed */


     /* set PORTA pin 7 as output to drive the speaker */
		//GPIO_PORTA_DIR_R = 0x80;
     /* set PORTA pin 7 as a digital pin */
		//GPIO_PORTA_DEN_R = 0x80;
		// The code for the countdown timer. Hint: a for loop
		//for(i = 15; i >= 0 ; i--){
		//	GPIO_PORTB_DATA_R = LEDPattern[i];
		//	SysTick_Wait200ms(2);
		//}
		
/*		
		// after the timed countdown, start the DC motor with the duty cycle to be 80%.
		while (1){
					/
		}
*/		
}
