#include "tm4c123gh6pm.h"

#define RS 1	// 0x01, PB0
#define RW 2	// 0x02, PB1
#define EN 4	// 0x04, PB2

void LCD_4Bits_Init(void);
void LCD_Write4Bits(unsigned char data, unsigned char control);
void LCD4Bits_Cmd(unsigned char command);
void LCD4Bits_Data(unsigned char data);
void LCD_OutString(char *pt);	// a new function to output a string
void delay_micro(int n);
void delay_milli(int n);