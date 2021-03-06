#include <MKL25Z4.H>
#include "user_defs.h"
#include "LEDs.h"
#include "gpio_aj.h"
#include "sim_aj.h"
#include "spi_aj.h"

extern void Auto_Setup(uint8_t type);

volatile uint32_t msTicks;                            /* counts 1ms timeTicks */
/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;                        /* increment counter necessary in Delay() */
}

/*------------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
 *------------------------------------------------------------------------------*/
void Delay (uint32_t dlyTicks) {
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) 
{
	Init_SIM();
	Init_GPIO();
  Init_RGB_LEDs();
	Init_SPI1();
	SysTick_Config(10000);
	Auto_Setup(0);
	while (1)
	{ }
}





// *******************************ARM University Program Copyright � ARM Ltd 2013*************************************   
