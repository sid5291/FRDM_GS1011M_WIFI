#include <MKL25Z4.H>
#include "user_defs.h"
#include "adc_aj.h"



void Init_PIT(unsigned period) {
	// Enable clock to PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	
	// Enable module, freeze timers in debug mode
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	//PIT->MCR |= PIT_MCR_FRZ_MASK;
	
	// Initialize PIT0 to count down from argument 
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(period);

	// No chaining
	PIT->CHANNEL[0].TCTRL &= PIT_TCTRL_CHN_MASK;
	
	// Generate interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;

	/* Enable Interrupts */
	NVIC_SetPriority(PIT_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(PIT_IRQn); 
	NVIC_EnableIRQ(PIT_IRQn);	
}

void PIT_IRQHandler() {
////	NVIC_ClearPendingIRQ(PIT_IRQn);
////	//PTD->PCOR = MASK(BLUE_LED_POS);
////	PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;
////	PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
////	channel=9;
////	ADC0->SC2 |= ADC_SC2_ACFE_MASK|ADC_SC2_ACREN_MASK;
////	ADC0->CV2=UpperBound;
////	ADC0->CV1=LowerBound;
////	ADC0->SC1[0] = 0x49;
}

