#include <MKL25Z4.H>
#include "user_defs.h"
#include "adc_aj.h"
#include "pwm_aj.h"
#include "sim_aj.h"



void Init_PWM()
{
	
// clock initialization in tpm_sim()  located in sim_aj.c
	
	tpm_sim();
	

	// TPM0 Ch 4 to PTE31
	PORTE->PCR[31] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[31] |= PORT_PCR_MUX(0x03);
	
	//TPM1 Ch0 to PTB0
	PORTB->PCR[0] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[0] |= PORT_PCR_MUX(0x03);
	//TPM1 Ch1 to PTB1
	PORTB->PCR[1] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[1] |= PORT_PCR_MUX(0x03);

	//set TPM to up-down and divide by 1 prescaler and clock mode
	TPM0->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(0));
	TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(0));
	
	//load the counter and mod
	TPM0->MOD = PWM_PERIOD;
	TPM1->MOD = PWM_PERIOD;
		
	//set channels to edge aligned high-true PWM
	TPM0->CONTROLS[4].CnSC |= TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
	TPM1->CONTROLS[1].CnSC |= TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;


	
	//set trigger mode
	TPM0->CONF |= TPM_CONF_TRGSEL(0x8)|TPM_CONF_DBGMODE(0x03);
	TPM1->CONF |= TPM_CONF_TRGSEL(0x9)|TPM_CONF_DBGMODE(0x03);
	
	//Set duty cycle to 50%;
	TPM0->CONTROLS[4].CnV = PWM_PERIOD>>2;
	TPM1->CONTROLS[0].CnV = PWM_PERIOD>>2;
	TPM1->CONTROLS[1].CnV = PWM_PERIOD>>2;
	
	
	NVIC_EnableIRQ(TPM0_IRQn);


}


void TPM0_IRQHandler(){
	PTC->PSOR |= MASK(17);  //toggle PTC 17
	NVIC_ClearPendingIRQ(TPM0_IRQn);
	TPM0->SC|=TPM_SC_TOF_MASK;

//	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
	//PTD->PSOR = MASK(BLUE_LED_POS);
}
