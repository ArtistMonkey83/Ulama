#include <stdint.h>
#include "LCD.h"
#include "tm4c123gh6pm.h"

void LCD_4Bits_Init(void){
	SYSCTL_RCGCGPIO_R |= (1U<<1);	// PB
	GPIO_PORTB_DIR_R = 0xFF;
	GPIO_PORTB_DEN_R = 0xFF;
	LCD4Bits_Cmd(0x28);	// 4-bit data, 2 lines, 5*7 fonts
	LCD4Bits_Cmd(0x06);	// increment the cursor automatically
	LCD4Bits_Cmd(0x0F);	// turn on the display
	LCD4Bits_Cmd(0x01);	// clear the display
	
}

void LCD_Write4Bits(unsigned char data, unsigned char control){
	data &= 0xF0;
	control &= 0x0F;
	GPIO_PORTB_DATA_R = data | control;
	GPIO_PORTB_DATA_R = data | control | EN;
	delay_micro(3);
	GPIO_PORTB_DATA_R = data;
	
}

void LCD4Bits_Cmd(unsigned char command){
	LCD_Write4Bits(command & 0xF0, 0);	// the control bit is 0 because this is a command not an actual text data.
	LCD_Write4Bits(command << 4, 0);	// the lower 4 bits of the command.
	if(command<4)
		delay_milli(2);
	else
		delay_micro(37);
}

void LCD4Bits_Data(unsigned char data){
	LCD_Write4Bits(data & 0xF0, RS);	// the control bit is 1 because this is to select the data register.
	LCD_Write4Bits(data << 4, RS);	// the lower 4 bits of the text data.
	delay_micro(37);
}

void LCD_OutString(char *pt){
  while(*pt){
    LCD4Bits_Data(*pt);
		delay_milli(500);
    pt++;
  }
}

void delay_micro(int n){
	int i, j;
	for(i=0; i<n; i++){
		for(j=0;j<3;j++){
		}
	}
}

void delay_milli(int n){
	int i, j;
	for(i=0; i<n; i++){
		for(j=0;j<3180;j++){
		}
	}
}