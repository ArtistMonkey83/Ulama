//First attempt
#include "TM4C123GH6PM.h"  // Include the TIVA TM4C123 header file

#define SLAVE_ADDR 0x28     // Slave address of the NFC reader
#define GPIOC_BASE 0x40006000   // Base address of GPIO Port C
#define GPIOC_DATA_OFFSET 0x3FC  // Offset for GPIO Data register
#define GPIOC_DIR_OFFSET 0x400   // Offset for GPIO Direction register

// Initialize I2C communication
void I2C_Init() {
  // Enable I2C0 clock and GPIOB clock
  SYSCTL->RCGCI2C |= SYSCTL_RCGCI2C_R0;
  SYSCTL->RCGCGPIO |= SYSCTL_RCGCGPIO_R1;

  // Configure PB2 and PB3 pins as I2C0 pins
  GPIOB->AFSEL |= (1 << 2) | (1 << 3);
  GPIOB->PCTL |= (3 << (2*4)) | (3 << (3*4));
  GPIOB->DEN |= (1 << 2) | (1 << 3);

  // Configure I2C0
  I2C0->MCR = 0x10;  // Enable Master mode
  I2C0->MTPR = 7;    // Set I2C clock speed to 100 Kbps
}

// Send data to the NFC reader
void I2C_Send(uint8_t addr, uint8_t data) {
  // Send start condition
  I2C0->MSA = (addr << 1) | 0;  // Set slave address and write mode
  I2C0->MDR = data;             // Set data to be sent
  I2C0->MCS = 3;                // Send start condition, transmit data, and stop condition
  while (I2C0->MCS & 1) {}      // Wait for the I2C bus to be idle
}

// Read data from the NFC reader
uint8_t I2C_Receive(uint8_t addr) {
  // Send start condition
  I2C0->MSA = (addr << 1) | 1;  // Set slave address and read mode
  I2C0->MCS = 3;                // Send start condition, receive data, and stop condition
  while (I2C0->MCS & 1) {}      // Wait for the I2C bus to be idle
  return I2C0->MDR;             // Return received data
}

// Initialize GPIO Port C
void GPIO_Init() {
  // Enable GPIOC clock
  SYSCTL->RCGCGPIO |= SYSCTL_RCGCGPIO_R2;

  // Set PC0-7 as output pins
  GPIOC->DIR |= 0xFF;
  GPIOC->DEN |= 0xFF;
}

int main() {
  // Initialize I2C and GPIO
  I2C_Init();
  GPIO_Init();

  // Send command to NFC reader to read data from tag
  I2C_Send(SLAVE_ADDR, 0x30);

  // Read data from NFC reader
  uint8_t data = I2C_Receive(SLAVE_ADDR);

  // Write data to GPIO Port C
  GPIOC->DATA = data;

  while(1);
}



//Second attempt
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#define I2C_ADDRESS 0x27 // NFC reader address
#define GPIO_PIN_C4 0x10 // GPIO Port C Pin 4

uint32_t sysClockFreq;

void initClock(void);
void initGPIO(void);
void initI2C(void);
void sendI2C(uint8_t data);
void receiveI2C(uint8_t *data);

int main(void) {
    uint8_t nfcData[1];
    initClock();
    initGPIO();
    initI2C();

    while (1) {
        receiveI2C(nfcData);
        if (nfcData[0] == 0x01) {
            // NFC reader detected tag
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_C4, GPIO_PIN_C4);
        } else {
            // NFC reader not detecting tag
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_C4, 0);
        }
    }
}

void initClock(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    sysClockFreq = SysCtlClockGet();
}

void initGPIO(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC));
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_C4);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_C4, 0);
}

void initI2C(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C1);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C1));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA6_I2C1SCL);
    GPIOPinConfigure(GPIO_PA7_I2C1SDA);
    GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
    GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);

    I2CMasterInitExpClk(I2C1_BASE, sysClockFreq, true);
    I2CMasterSlaveAddrSet(I2C1_BASE, I2C_ADDRESS, false);
    I2CMasterTimeoutSet(I2C1_BASE, sysClockFreq / 100); // 10ms timeout
}

void sendI2C(uint8_t data) {
    I2CMasterDataPut(I2C1_BASE, data);
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    while (I2CMasterBusy(I2C1_BASE));
}

void receiveI2C(uint8_t *data) {
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
    while (I2CMasterBusy(I2C1_BASE));
    *data = I2CMasterDataGet(I2C1_BASE);
}

//Third attempt
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#define I2C_ADDRESS 0x27 // NFC reader address
#define GPIO_PIN_C4 0x10 // GPIO Port C Pin 4

uint32_t sysClockFreq;
uint8_t ledColors[6][3] = {{255, 0, 0}, {255, 127, 0}, {255, 255, 0}, {0, 255, 0}, {0, 0, 255}, {75, 0, 130}};
uint8_t tagData[16];
uint8_t readData[16];

void initClock(void);
void initGPIO(void);
void initI2C(void);
void sendI2C(uint8_t data);
void receiveI2C(uint8_t *data);
void writeTagData(uint8_t *data);
void readTagData(uint8_t *data);
void delayMs(uint32_t ms);
void rainbowEffect(void);
void dimEffect(void);

int main(void) {
    uint8_t nfcData[1];
    initClock();
    initGPIO();
    initI2C();

    while (1) {
        receiveI2C(nfcData);
        if (nfcData[0] == 0x01) {
            // NFC reader detected tag
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_C4, GPIO_PIN_C4);
            readTagData(readData);
            dimEffect();
        } else if (nfcData[0] == 0x02) {
            // NFC reader writing tag
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_C4, GPIO_PIN_C4);
            writeTagData(tagData);
            rainbowEffect();
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_C4, 0);
            delayMs(250);
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_C4, GPIO_PIN_C4);
            delayMs(250);
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_C4, 0);
            delayMs(250);
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_C4, GPIO_PIN_C4);
            delayMs(250);
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_C4, 0);
            delayMs(250);
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_C4, GPIO_PIN_C4);

//Fourth attempt
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#define I2C_ADDRESS 0x27 // NFC reader address
#define GPIO_PIN_C4 0x10 // GPIO Port C Pin 4
#define LED_COUNT 10     // Number of addressable LEDs connected
#define LED_BRIGHTNESS_STEP 10 // Step size for brightness levels

uint32_t sysClockFreq;
uint8_t tagData[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void initClock(void);
void initGPIO(void);
void initI2C(void);
void sendI2C(uint8_t data);
void receiveI2C(uint8_t *data);
void writeTagData(void);
void readTagData(void);
void rainbowEffect(void);

int main(void) {
    initClock();
    initGPIO();
    initI2C();

    while (1) {
        // Check for NFC tag present
        receiveI2C(&tagData[0]);
        if (tagData[0] == 0x01) {
            // NFC tag detected
            rainbowEffect();
            writeTagData();
            // Turn off all LEDs
            for (int i = 0; i < LED_COUNT; i++) {
                GPIOPinWrite(GPIO_PORTC_BASE, 1 << i, 0);
            }
            // Blink LEDs to indicate writing is complete
            for (int i = 0; i < 2; i++) {
                SysCtlDelay(sysClockFreq / 4);
                for (int j = 0; j < LED_COUNT; j++) {
                    GPIOPinWrite(GPIO_PORTC_BASE, 1 << j, 1 << j);
                }
                SysCtlDelay(sysClockFreq / 4);
                for (int j = 0; j < LED_COUNT; j++) {
                    GPIOPinWrite(GPIO_PORTC_BASE, 1 << j, 0);
                }
            }
        } else {
            // No NFC tag detected
            readTagData();
        }
    }
}

void initClock(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    sysClockFreq = SysCtlClockGet();
}

void initGPIO(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC));
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, (1 << LED_COUNT) - 1); // All Port C pins are outputs
}

void initI2C(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C1);
    while (!SysCtlPeripheralReady(SYSCTL_PER

//Fifth attempt
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"

#define I2C_ADDRESS 0x27 // NFC reader address
#define GPIO_PIN_C4 0x10 // GPIO Port C Pin 4 (LEDs)

uint32_t sysClockFreq;

// RGB LED color values
struct RGBColor {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

// Rainbow colors
const RGBColor rainbow[] = {
    {255, 0, 0},   // Red
    {255, 127, 0}, // Orange
    {255, 255, 0}, // Yellow
    {0, 255, 0},   // Green
    {0, 0, 255},   // Blue
    {75, 0, 130},  // Indigo
    {143, 0, 255}  // Violet
};
const uint8_t numColors = sizeof(rainbow) / sizeof(RGBColor);

void initClock(void);
void initGPIO(void);
void initI2C(void);
void sendI2C(uint8_t data);
void receiveI2C(uint8_t *data);
void writeNfcTag(void);
void readNfcTag(void);
void dimLEDs(void);
void brightenLEDs(void);
void setLEDColor(RGBColor color);

int main(void) {
    initClock();
    initGPIO();
    initI2C();
    uint8_t nfcData[16];
    bool writingTag = false;
    bool readingTag = false;

    while (1) {
        receiveI2C(nfcData);
        if (nfcData[0] == 0x01 && !writingTag && !readingTag) {
            // NFC reader detected tag and not currently writing or reading
            writingTag = true;
            writeNfcTag();
            writingTag = false;
        } else if (nfcData[0] == 0x02 && !writingTag && !readingTag) {
            // NFC reader detected tag and not currently writing or reading
            readingTag = true;
            readNfcTag();
            readingTag = false;
        }
    }
}

void initClock(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    sysClockFreq = SysCtlClockGet();
}

void initGPIO(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC));
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_C4);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_C4, 0);
}

void initI2C(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C1);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C1));

    SysCtlPeripheralEnable

//Six attempt
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"

#define I2C_ADDRESS 0x27 // NFC reader address
#define GPIO_PIN_C4 0x10 // GPIO Port C Pin 4 (LEDs)

uint32_t sysClockFreq;

// RGB LED color values
struct RGBColor {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

// Rainbow colors
const RGBColor rainbow[] = {
    {255, 0, 0},   // Red
    {255, 127, 0}, // Orange
    {255, 255, 0}, // Yellow
    {0, 255, 0},   // Green
    {0, 0, 255},   // Blue
    {75, 0, 130},  // Indigo
    {143, 0, 255}  // Violet
};
const uint8_t numColors = sizeof(rainbow) / sizeof(RGBColor);

void initClock(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    sysClockFreq = SysCtlClockGet();
}

void initGPIO(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC));
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_C4);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_C4, 0);
}

void initI2C(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C1);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C1));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
    GPIOPinConfigure(GPIO_PA6_I2C1SCL);
    GPIOPinConfigure(GPIO_PA7_I2C1SDA);
    GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    I2CMasterInitExpClk(I2C1_BASE, sysClockFreq, true);
    I2CMasterSlaveAddrSet(I2C1_BASE, I2C_ADDRESS, false);
}

void sendI2C(uint8_t data) {
    I2CMasterDataPut(I2C1_BASE, data);
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    while (I2CMasterBusy(I2C1_BASE));
}

void receiveI2C(uint8_t *data) {
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
    while (I2CMasterBusy(I2C1_BASE));
    *data = I2CMasterDataGet(I2C1_BASE);
}

void writeNfcTag(void) {
    // Set LED to rainbow mode
    uint8_t colorIndex = 0;
    while (writingTag) {
        setLEDColor(rainbow[colorIndex]);
        colorIndex++;
        if

//Seventh attempt
void writeNfcTag(void) {
    for (uint8_t i = 0; i < 16; i++) {
        sendI2C(i + 1); // send block number to NFC reader
        sendI2C(0x00); // send data to write to NFC tag
    }

    // Marching rainbow effect on LEDs while writing NFC tag
    uint8_t colorIndex = 0;
    while (colorIndex < numColors) {
        setLEDColor(rainbow[colorIndex]);
        colorIndex++;
        if (colorIndex == numColors) {
            colorIndex = 0;
        }
        SysCtlDelay(sysClockFreq / 3); // delay 1 second
    }

    // Turn off LEDs for 0.25 seconds and then on for 0.25 seconds
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_C4, 0);
    SysCtlDelay(sysClockFreq / 4); // delay 0.25 seconds
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_C4, GPIO_PIN_C4);
    SysCtlDelay(sysClockFreq / 4); // delay 0.25 seconds
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_C4, 0);
    SysCtlDelay(sysClockFreq / 4); // delay 0.25 seconds
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_C4, GPIO_PIN_C4);
    SysCtlDelay(sysClockFreq / 4); // delay 0.25 seconds
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_C4, 0);
}

void readNfcTag(void) {
    // Dim LEDs and then gradually brighten them when reading NFC tag
    for (uint8_t i = 0; i <= 255; i++) {
        PWMOutputState(PWM0_BASE, PWM_OUT_5_BIT, true);
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5, round((i / 255.0) * PWMGenPeriodGet(PWM0_BASE, PWM_GEN_2)));
        SysCtlDelay(sysClockFreq / 10000); // delay 0.1 ms
    }

    for (uint8_t i = 0; i < 16; i++) {
        sendI2C(i + 1); // send block number to NFC reader
        receiveI2C(NULL); // read data from NFC tag
    }

    // Brighten LEDs to maximum when reading NFC tag is complete
    for (uint8_t i = 255; i >= 0; i--) {
        PWMOutputState(PWM0_BASE, PWM_OUT_5_BIT, true);
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5, round((i / 255.0) * PWMGenPeriodGet(PWM0_BASE, PWM_GEN_2)));
        SysCtlDelay(sysClockFreq / 10000); // delay 0.1 ms
    }
}

void dimLEDs(void) {
    PWMOutputState(PWM0_BASE, PWM_OUT_5_BIT, true);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5, 0);
}

void brightenLEDs(void) {
    PWMOutputState(PWM0_BASE, PWM_OUT_5_BIT, true);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5, PWMGenPeriodGet(PWM0_BASE, PWM_GEN_2));
}

void setLEDColor(RGBColor color) {
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2

//Eight attempt
/*
Note that the dimLEDs and brightenLEDs functions use PWM to gradually decrease and increase the brightness of the LEDs, respectively. PWM is set up in the main function and assumed to be configured as follows:

PWM1 (base): GPIO Port C
PWM_OUT_4: GPIO Pin C4 (Red LED)
PWM_OUT_5: GPIO Pin C5 (Green LED)
PWM_OUT_6: GPIO Pin C6 (Blue LED)
Frequency: 100 Hz (default)
Duty Cycle: initially set to 0 (off) for all LEDs

*/



void writeNfcTag(void) {
    for (uint8_t i = 0; i < 16; i++) {
        sendI2C(i + 1); // send block number to NFC reader
        sendI2C(0x00);  // write 0x00 to block
        SysCtlDelay(sysClockFreq / 3); // wait for write to complete
    }

    // Marching rainbow effect
    for (uint8_t i = 0; i < numColors; i++) {
        setLEDColor(rainbow[i]);
        SysCtlDelay(sysClockFreq / 2);
    }
}

void readNfcTag(void) {
    dimLEDs(); // Start with dim LEDs

    for (uint8_t i = 0; i < 16; i++) {
        sendI2C(i + 1); // send block number to NFC reader
        uint8_t data;
        receiveI2C(&data); // read data from block
        SysCtlDelay(sysClockFreq / 3); // wait for read to complete
    }

    brightenLEDs(); // Increase LED brightness
}

void dimLEDs(void) {
    for (uint8_t i = 255; i > 0; i--) {
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, i);
        SysCtlDelay(sysClockFreq / 1000);
    }
}

void brightenLEDs(void) {
    for (uint8_t i = 0; i < 255; i++) {
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, i);
        SysCtlDelay(sysClockFreq / 1000);
    }
}

void setLEDColor(RGBColor color) {
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_4, color.red * 255 / 255); // Red
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, color.green * 255 / 255); // Green
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, color.blue * 255 / 255); // Blue
}

/*
workflow under consideration
Initialize clocks, GPIO ports A and C, and SPI on port B using the initClock(), initGPIO(), and initSPI() functions respectively.

Implement the following functions for controlling the NFC reader:

initI2C(): Initializes the I2C communication protocol for the NFC reader.
sendI2C(uint8_t data): Sends data to the NFC reader via I2C.
receiveI2C(uint8_t *data): Receives data from the NFC reader via I2C.
writeNfcTag(): Writes data to the NFC tag using the NFC reader and controls the LEDs on GPIO port C accordingly.
readNfcTag(): Reads data from the NFC tag using the NFC reader and controls the LEDs on GPIO port C accordingly.
dimLEDs(): Dims the LEDs on GPIO port C.
brightenLEDs(): Brightens the LEDs on GPIO port C.
setLEDColor(RGBColor color): Sets the color of the LEDs on GPIO port C.
Implement the following functions for controlling the TFT displays on SPI port B:
initSPI(): Initializes the SPI communication protocol for the TFT displays.
writeSPI(uint8_t data): Writes data to the TFT displays via SPI.
setLogo(int logo): Sets the logo to be displayed on the TFT displays based on the value of the "logo" variable.
setMelody(int melody): Sets the melody to be played based on the value of the "melodySwitch" variable.
melody1(): Plays "Happy Birthday" melody using pulse width modulation.
melody2(): Plays "Feliz Navidad" melody using pulse width modulation.
melody3(): Plays "Hot Cross Buns" melody using pulse width modulation.
melody4(): Plays "Pomp and Circumstance" melody using pulse width modulation.
Implement the following functions for controlling the speaker on GPIO pin A:
initPWM(): Initializes pulse width modulation (PWM) on GPIO pin A for controlling the speaker.
playMelody(int melody): Plays the selected melody using PWM.
Implement the following functions for controlling the LEDs on GPIO port C:
initGPIO(): Initializes GPIO port C for controlling the LEDs.
setLEDRed(): Sets the LEDs on GPIO port C to red.
setLEDGreen(): Sets the LEDs on GPIO port C to green.
setLEDBlue(): Sets the LEDs on GPIO port C to blue.
setLEDYellow(): Sets the LEDs on GPIO port C to yellow.
setLEDWhite(): Sets the LEDs on GPIO port C to white.
setLEDRainbow(): Sets the LEDs on GPIO port C to display a marching rainbow effect.
In the main() function, read the tag value and call the appropriate functions based on the values of the variables "logo", "melodySwitch", and "teamColors".

Other considerations:

Initialize the necessary hardware components such as GPIO ports, SPI protocol, I2C communication protocol, and speaker.
Set up the NFC reader to read and write NFC type 2 tags using I2C protocol.
Create variables for logo, melodyGoal, melodyTime, melodySwitch, and teamColors.
When a tag is read, evaluate the logo variable and display the corresponding logo on the TFT displays connected to port b.
Call the melodySwitch function and evaluate the melodySwitch variable to play the corresponding melody using pulse width modulation on the speaker.
When reading the tag value for teamColors, evaluate the teamColors variable and set the corresponding LED color on the addressable LEDs connected to port c.
When writing the NFC tag, display a marching rainbow effect on the LEDs connected to port c.
Once the tag has been written, stop the marching effect and display the corresponding LED sequence based on the teamColors variable.
Continue reading and writing tags as required.
Here are some sample code snippets for some of the required functions:

potential function skeletons to consider:

// Initializing GPIO ports, SPI protocol, I2C communication protocol, and speaker
void initHardware() {
  // Initialize GPIO ports A, B, and C
  // Initialize SPI protocol on port B
  // Initialize I2C communication protocol
  // Initialize speaker on GPIO pin A
}

// Set up NFC reader to read and write NFC type 2 tags using I2C protocol
void initNfcReader() {
  // Initialize I2C communication with NFC reader
  // Configure NFC reader for reading and writing NFC type 2 tags
}

// Read and evaluate tag variables
void readNfcTag() {
  // Read tag variables using NFC reader
  // Evaluate logo variable and display corresponding logo on TFT displays
  // Call melodySwitch function and evaluate melodySwitch variable to play corresponding melody on speaker
  // Evaluate teamColors variable and set corresponding LED color on addressable LEDs
}

// Write NFC tag with variables
void writeNfcTag(int logo, int melodyGoal, int melodyTime, int melodySwitch, int teamColors) {
  // Display marching rainbow effect on addressable LEDs
  // Write tag variables using NFC reader
  // Stop marching effect and display corresponding LED sequence based on teamColors variable
}


*/


//nfc 10

#include "TM4C123GH6PM.h"

#define SCREEN_ADDR 0x27
#define NFC_ADDR    0x50

// function to write a value to a register on an I2C device
void i2c_write(uint8_t addr, uint8_t reg, uint8_t data) {
    I2C1_MSA = addr << 1;
    I2C1_MDR = reg;
    I2C1_MCS = 0x03;
    while (I2C1_MCS & 0x01);
    I2C1_MDR = data;
    I2C1_MCS = 0x05;
    while (I2C1_MCS & 0x01);
}

// function to read a value from a register on an I2C device
uint8_t i2c_read(uint8_t addr, uint8_t reg) {
    I2C1_MSA = addr << 1;
    I2C1_MDR = reg;
    I2C1_MCS = 0x03;
    while (I2C1_MCS & 0x01);
    I2C1_MSA = (addr << 1) | 0x01;
    I2C1_MCS = 0x07;
    while (I2C1_MCS & 0x01);
    return I2C1_MDR;
}

// function to read the NFC tag
void read_nfc_tag() {
    uint8_t team_colors = i2c_read(NFC_ADDR, 0x01);
    uint8_t melody_goal = i2c_read(NFC_ADDR, 0x02);
    uint8_t melody_time = i2c_read(NFC_ADDR, 0x03);
    uint8_t melody_switch = i2c_read(NFC_ADDR, 0x04);
    uint8_t logo = i2c_read(NFC_ADDR, 0x05);

    if (team_colors < 1 || team_colors > 4 ||
        melody_goal < 1 || melody_goal > 4 ||
        melody_time < 1 || melody_time > 4 ||
        melody_switch < 1 || melody_switch > 4 ||
        logo < 1 || logo > 4) {
        i2c_write(SCREEN_ADDR, 0x00, 'N');
        i2c_write(SCREEN_ADDR, 0x01, 'o');
        i2c_write(SCREEN_ADDR, 0x02, 'n');
        i2c_write(SCREEN_ADDR, 0x03, ' ');
        i2c_write(SCREEN_ADDR, 0x04, 'c');
        i2c_write(SCREEN_ADDR, 0x05, 'o');
        i2c_write(SCREEN_ADDR, 0x06, 'm');
        i2c_write(SCREEN_ADDR, 0x07, 'p');
        i2c_write(SCREEN_ADDR, 0x08, 'a');
        i2c_write(SCREEN_ADDR, 0x09, 't');
        i2c_write(SCREEN_ADDR, 0x0A, 'i');
        i2c_write(SCREEN_ADDR,
//11
#include <TM4C123.h>

// I2C configuration
#define I2C_PORT                GPIOB
#define I2C_SDA                 3
#define I2C_SCL                 2
#define I2C_SPEED               100000

// GPIO configuration
#define GPIO_PORTA              GPIOA
#define GPIO_PORTB              GPIOB
#define GPIO_PORTC              GPIOC

// NFC variables
int teamColors;
int melodyGoal;
int melodyTime;
int melodySwitch;
int logo;

// Error message
char error_message[] = "Non compatible Ulama NFC!";

int main(void)
{
    // Initialize I2C
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    GPIOPinTypeI2CSCL(I2C_PORT, I2C_SCL);
    GPIOPinTypeI2C(I2C_PORT, I2C_SDA);
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true);
    I2CMasterEnable(I2C0_BASE);
    I2CMasterSlaveAddrSet(I2C0_BASE, 0x00, false);
    I2CMasterDataPut(I2C0_BASE, 0x00);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    while (I2CMasterBusy(I2C0_BASE));

    // Initialize GPIO
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPinTypeGPIOOutput(GPIO_PORTA, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTC, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);

    // Read NFC tag
    I2CMasterSlaveAddrSet(I2C0_BASE, 0x28, false);
    I2CMasterDataPut(I2C0_BASE, 0x00);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    while (I2CMasterBusy(I2C0_BASE));
    I2CMasterSlaveAddrSet(I2C0_BASE, 0x28, true);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
    while (I2CMasterBusy(I2C0_BASE));
    teamColors = I2CMasterDataGet(I2C0_BASE);
    melodyGoal = I2CMasterDataGet(I2C0_BASE);
    melodyTime = I2CMasterDataGet(I2C0_BASE);
    melodySwitch = I2CMasterDataGet(I2C0_BASE);
    logo = I2CMasterDataGet(I2C0_BASE);
//12
#include "TM4C123.h"

#define I2C_SLAVE_ADDRESS 0x50
#define I2C_FREQUENCY 100000
#define I2C_TIMEOUT 1000

#define SCREEN_PORT GPIO_PORTA_BASE
#define SCREEN_PIN GPIO_PIN_0

#define NFC_PORT GPIO_PORTB_BASE
#define NFC_SDA_PIN GPIO_PIN_2
#define NFC_SCL_PIN GPIO_PIN_3

int main() {
    // Initialize I2C module
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_3);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_2);
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true);

    // Initialize screen
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOOutput(SCREEN_PORT, SCREEN_PIN);

    // Initialize NFC reader
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOOutput(NFC_PORT, NFC_SDA_PIN | NFC_SCL_PIN);

    // Write tag with variables
    uint8_t writeBuffer[5];
    writeBuffer[0] = teamColors;
    writeBuffer[1] = melodyGoal;
    writeBuffer[2] = melodyTime;
    writeBuffer[3] = melodySwitch;
    writeBuffer[4] = logo;
    I2CMasterSlaveAddrSet(I2C0_BASE, I2C_SLAVE_ADDRESS, false);
    I2CMasterDataPut(I2C0_BASE, writeBuffer[0]);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    while(I2CMasterBusy(I2C0_BASE)) {}
    for(int i = 1; i < 5; i++) {
        I2CMasterDataPut(I2C0_BASE, writeBuffer[i]);
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
        while(I2CMasterBusy(I2C0_BASE)) {}
    }
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_STOP);
    while(I2CMasterBusy(I2C0_BASE)) {}

    // Read tag and check variables
    uint8_t readBuffer[5];
    I2CMasterSlaveAddrSet(I2C0_BASE, I2C_SLAVE_ADDRESS, true);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
    while(I2CMasterBusy(I2C0_BASE)) {}
    for(int i = 0; i < 5; i++) {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
        while(I2CMasterBusy(I2C0_BASE)) {}
        readBuffer[i] = I2
