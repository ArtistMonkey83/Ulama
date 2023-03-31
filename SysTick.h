#include <stdint.h>
// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void);

// Time delay using busy wait.
// The delay parameter is in units of the core clock. 
void SysTick_Wait(uint32_t delay);

// This assumes 80 MHz system clock.
void SysTick_Wait200ms(uint32_t delay);

void SysTick_Wait02ms(uint32_t delay);

void SysTick_Wait08ms(uint32_t delay);