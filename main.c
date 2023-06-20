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

int goal();					//Fucntion to determine state of goals, was one made==1 or no goal==0
int pwrDwn();				//Function to determine if the game should perform power down protocols
int reset();				//Function to determine if reset the game
int shotClock();		//Function to track the time remaining for a player to take a shot, 5 minute maximum
int gameTimer();		//Function to track the overall time remaining till game is over, 30 minute maximum
int ulamaTimeout();	//Function to automatically turn off the game after 33 minutes, 3 minutes after the end of the game.

void welcome();			//Function to display a welcome message
void goodBye();			//Function to display a goodbye message
void goalScored();	//Function to display Goal message
void display();			//Function to display player name

int SHOTCLOCK = 0;	//Global variable for how much time a player has left for their turn
int GAMETIMER = 0;	//Global variable for how much time is left in the currrent game
int TIMEOUT = 0;		//Global variable for the timeout signal
int PLAYER = 0; 		//Global variable to hold the current player, 0 == player 1, 1 == player 2
int TURNS = 0;			//Global variable to hold the number of turns taken

int main(){
		char player1[9] = {'P','l','a','y','e','r',' ','1','\0'}; //c-string to hold player1 name
		char player2[9] = {'P','l','a','y','e','r',' ','2','\0'};	//c-string to hold player2 name
		
		int goalCounter[2] = {0, 0};		//Array to hold the number of goals made by all players
	
		PLL_Init();			/* call the function to initialize PLL*/
		SysTick_Init(); /* call the function to initialize SysTick*/
	
/* enable clock to GPIOA */
/* enable clock to GPIO B and C */
/* PortB, Pin0 == Goal, Green LEDs flash when button pushed OUTPUT*/
/* PortB, Pin1 == On/off, Red LEDs flash when switch activated/deactivated OUTPUT*/
/* PortC, Pin4 simulation of displacement sensor as a switch INPUT*/
/* PortC, Pin7 controls speaker tone OUTPUT*/

		SYSCTL_RCGCGPIO_R = PB | PC;	/* Enable PORTB & PORTC clock*/
		GPIO_PORTB_DIR_R = 0x03;			/* set PORTB pins 0 & 1 as digital OUTPUT pins*/
		GPIO_PORTB_DEN_R = 0x03;			/* Enable PORTB pins*/
		
		GPIO_PORTC_DIR_R = 0x80;			/* set PORTC pin4 as INPUT (turning on switch simulating displacement sensor) and 7 as OUTPUT (speaker)*/
		GPIO_PORTC_DEN_R = 0x90;			/* Enable PORTC pins*/
						
		LEDon(); 			/* Turn on the LEDs*/
		welcome();		/* Display Welcome Message on screen*/
		SpeakerOn();  /* Play boot up tones on speaker*/	
		display();		/* Display player name*/
		
/*pwrDwn and reset are not activated we play game*/
		if(pwrDwn() == 0 && reset() == 0){
			if(gameTimer()< 31){			/* If there is still time left in the game...*/
					if(shotClock() < 6){	/* If the player still has time to shoot...*/
						if(goal()== 1){
							if( PLAYER >= 0 && PLAYER < 2)	/* Check for a vaild player*/
							goalCounter[PLAYER]++;					/* Update current player's goal counter*/
							
						}//if goal
					}//if shotclock
			}//if gametimer
		}//if pwrDwn and reset ==0
		
/*pwrDwn not activated but reset IS, we reset the game*/		
		if(pwrDwn() == 0 && reset() == 1){
			goalCounter[0] = 0; /* Clear any previously recorded goals*/
			goalCounter[1] = 0;
			LEDon(); 			/* Turn on the LEDs*/
			welcome();		/* Display Welcome Message on screen*/
			SpeakerOn();  /* Play boot up tones on speaker*/	
			display();		/* Display player name*/
			SHOTCLOCK = 0;/* Reset shotclock*/
			GAMETIMER = 0;/* Reset game timer*/
			TIMEOUT = 0;	/* Reset the timeout timer*/
			PLAYER = 0;		/* Reset to player 1*/
			TURNS = 0;		/* Reset the number of turns taken*/
		}//if pwrDwn=0 and reset=1
/*pwrDwn is activated we turn off everything*/
		if(pwrDwn() == 1){
			LEDoff();					//Turn off the LEDs
			goodBye();				//Display goodbye message
			SpeakerOff();			//Emit goodbye tone
			
			return 0;					//Exit main function
		}
}//main

/*Fucntion to determine state of goals, was one made==1 or no goal==0?*/
int goal(){
/* Polling to see if there is a goal, switch activated on PORTC pin4*/

	return 0;							//Exit function
}


/*Function to determine if the game should perform power down protocols*/
int pwrDwn(){
	int PwrDwn = 0; /* Is there incomming signal from on/off button?*/
	return PwrDwn;	/* Power down Protocols hard coded to be deactivated*/
}


/*Function to assess if players want to reset the game*/
int reset(){

	return 0;							//Exit function
}


/*Function to track the time remaining for a player to take a shot, 5 minute maximum*/
int shotClock(){
		int i,j;
		for(i=0; i<60000; i++){	//About 1 minute 
			for(j=0;j<3180;j++){ //About 1 millisecond
			}
		}
		SHOTCLOCK++; //Increment the shotclock by 1 minute, this happens when if statement compares the variable						
	
		if(SHOTCLOCK == 5){	//Switch to next player's turn
			switch(PLAYER){
				case 0: PLAYER = 1;
								break;
				case 1: PLAYER = 0;
								TURNS = 1;	//When we switch back to player 1 there has been 1 turn completed by each player
								break;
			}//switch case statement	
		}//if shotclock == 5	
		return 0;							//Exit function
}//END shotclock function




/*Function to track the overall time remaining till game is over, 30 minute maximum*/
int gameTimer(){
		if(TURNS < 3){ //we still have time left in the current game
		
		}//if
		return 0;							//Exit function
}//END game timer function


/*Function to automatically turn off the game after 33 minutes, 3 minutes after the end of the game*/
int ulamaTimeout(){
	if(TURNS == 3){
		int i,j;
		for(i=0; i<60000; i++){	//About 1 minute 
			for(j=0;j<3180;j++){ //About 1 millisecond
			}
		}
	}//if
	return 0;							//Exit function
}

/*Function to display a welcome message*/
void welcome(){

}			

/*Function to display a goodbye message*/
void goodBye(){

}

/*Function to display Goal message*/
void goalScored(){

}

/*Function to display player name*/
void display(){

}