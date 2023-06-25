#include "GPIO.h"
#include <stdint.h>

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

/* Port_B, Pin0 == Goal, Green LEDs flash when button pushed OUTPUT*/
/* Port_B, Pin1 == On/off, Red LEDs flash when switch activated/deactivated OUTPUT*/
		void PortB_Init(void) {
			SYSCTL_RCGCGPIO_R |= PB;			/* 0000 0010, enable clock to GPIO B*/
			GPIO_PORTB_DIR_R = 0x03;			/* set PORT_B pins 0 & 1 as digital OUTPUT pins*/
		  GPIO_PORTB_DEN_R = 0x03;			/* Enable PORT_B pins*/
		}


/* Port_C, Pin4 simulation of displacement sensor as a switch INPUT*/
/* Port_C, Pin7 controls speaker tone OUTPUT*/		
		void PortC_Init(void){
			SYSCTL_RCGCGPIO_R |= PC;			/* 0000 0100, enable clock to GPIO C*/
			GPIO_PORTC_DIR_R = 0x80;			/* set PORT_C pin4 as INPUT (turning on switch simulating displacement sensor) and 7 as OUTPUT (speaker)*/
			GPIO_PORTC_DEN_R = 0x90;			/* Enable PORT_C pins*/
		}
		

/* Port_F has psecial function, need to unlock to modify */
		void PortF_Init(void) { volatile unsigned long delay;
			SYSCTL_RCGCGPIO_R |= PF;			/* 1000 0000, enable clock to GPIO F*/
			
			GPIO_PORTF_LOCK_R = 0x4C4F434B;
			GPIO_PORTF_CR_R = 0x01;
			GPIO_PORTF_LOCK_R = 0;
			
			GPIO_PORTF_DIR_R &= ~0x11;		/* F4 & F0 for inputs for switch, 0001 0001*/
			GPIO_PORTF_DEN_R |= 0x1F;			/* Make pins 4-0 digital pins*/
			GPIO_PORTF_PUR_R |= 0x11;			/* Enable pull up resistor for Pins 4, 0 */	
			
			GPIO_PORTF_IS_R &= ~0x11;			/* Make bit 4,0 edge sensitive*/
			GPIO_PORTF_IBE_R &= ~0x11;		/* Trigger is controlled by IEV*/
			GPIO_PORTF_IEV_R &= ~0x11;		/* Falling edge trigger*/
			GPIO_PORTF_ICR_R |= ~0x11;		/* Clear any prior interrupt*/
			GPIO_PORTF_IM_R |= ~0x11;			/* Unmask interrupt*/
			
			NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | (5<<21);			/* Set interrupt priority to 5*/
			NVIC_EN0_R |= 0x40000000;			/* Enable IRQ30 (D30 of ISER[0])*/
			
			__enable_irq();								/* Global enable of IRQs*/
		}