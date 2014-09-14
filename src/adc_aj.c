#include <MKL25Z4.H>
#include "user_defs.h"
#include "LEDs.h"
#include "control_aj.h"
#include "sim_aj.h"

#define R1 (1e6)
#define R2 (1e6)
#define SCALE_FACTOR ((R1+R2)/(R2))


uint16_t d;
uint32_t result=0;
int16_t error=0;
uint8_t ADC_channel=0;
int flag=0;

uint32_t D_local = D_MAX>>2;

void Init_ADC(void) {

		adc_sim();

	// set PTE20 as ADC0_SE0 -- DEFAULT
	PORTE->PCR[20] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[20] |= PORT_PCR_MUX(0);
	
		// set PTE21 as ADC0_SE4a -- DEFAULT
	PORTE->PCR[21] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[21] |= PORT_PCR_MUX(0);
	
		// set PTE22 as ADC0_SE3 -- DEFAULT
	PORTE->PCR[22] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[22] |= PORT_PCR_MUX(0);
	
	ADC0->SC1[0] = 0;
	ADC0->CFG1 = 0;
	ADC0->SC2 = 0;
	
	//Enable Interrupt and select ADC0SE0
	ADC0->SC1[0] |= ADC_SC1_AIEN_MASK|ADC_SC1_ADCH(0);
	
	// Low Power configuration and  16 bit unsigned single-ended conversion
	ADC0->CFG1 |= ADC_CFG1_ADLPC_MASK|ADC_CFG1_MODE(3); 
////	ADC0->CFG2 |= ADC_CFG2_ADHSC_MASK;

	ADC0->SC2 |= ADC_SC2_ADTRG_MASK;



	NVIC_EnableIRQ (ADC0_IRQn);
}


void ADC0_IRQHandler(){
////	NVIC_ClearPendingIRQ(ADC0_IRQn); 
////	PTD->PSOR = MASK(BLUE_LED_POS);
	


	
	    /* Start user code. Do not edit comment generated here */
#if FXP
    int32_t t;
#else
    volatile float error; // int16_t
#endif

      PTC->PCOR |= MASK(17);
			PTC->PSOR |= MASK(16);  //set PTC 16

			result= ADC0->R[0];
	
	switch(ADC_channel)
	{
		
		case 0:
		
////			//Make it Software Trigger
////			ADC0->SC2 = 0; //&~ADC_SC2_ADTRG_MASK;
////				//ADC0SE4
////			ADC0->SC1[0] = 0x44;		
		
      ctl_parms_2_7.e[1] = ctl_parms_2_7.e[0];
      
      t = ((uint32_t)6144<<16)/(result);  //(1.5 * 2^12 * 2^16)/(result) ==> 1.5*2^12 is in q12 FXP
					ctl_parms_2_7.e[0] = set_value_2_7 - (int32_t)t;           

      //// d[n] = d[n-1] + K1*e[n] + K2*e[n-1];
			ctl_parms_2_7.d[0] += K1_PI_MCU*ctl_parms_2_7.e[0];
      
			ctl_parms_2_7.d[0] += K2_PI_MCU*ctl_parms_2_7.e[1];
			      	
      
      if (ctl_parms_2_7.d[0] > D_MAX) {
        ctl_parms_2_7.d[0] = D_MAX;
      } 
			else if (ctl_parms_2_7.d[0] < D_MIN) {
        ctl_parms_2_7.d[0] = D_MIN;
      } 
			
			TPM0->CONTROLS[4].CnV = ctl_parms_2_7.d[0]>>16;
			
////			/**********Incremental Control******/	
////		error = 32767-result;
////	
////	if (t<set_value_2_7)
////		d++;
////	else if (t>set_value_2_7)
////		d--;
////	
////	if(d>920)
////	{
////		d=920;
////	}
////	if(d<20)
////	{
////		d=20;
////	}
////	
////	TPM0->CONTROLS[4].CnV = d;
/////***********************************/

			break;
			
		case 1:
			//ADC0SE0
			ADC0->SC1[0] = 0x43;
		
			ctl_parms_3_3.e[1] = ctl_parms_3_3.e[0];
      
////      t = set_value_2_7 - result;
					ctl_parms_3_3.e[0] = set_value_3_3 - (int32_t)result;
		
      
			ctl_parms_3_3.d[0] += K1_PI_28_4*ctl_parms_3_3.e[0];
      
			ctl_parms_3_3.d[0] += K2_PI_28_4*ctl_parms_3_3.e[1];
			
      	
      
      if (ctl_parms_3_3.d[0] > D_MAX) {
        ctl_parms_3_3.d[0] = D_MAX;
      } 
			else if (ctl_parms_3_3.d[0] < D_MIN) {
        ctl_parms_3_3.d[0] = D_MIN;
      } 
			
			TPM1->CONTROLS[0].CnV = ctl_parms_3_3.d[0]>>16;
			
			break;
		
		case 2:
			
			//Make it HW Trigger
			ADC0->SC2 = ADC_SC2_ADTRG_MASK;
			//ADC0SE0 and EI_ADC_INT
			ADC0->SC1[0] = 0x40;
		
			ctl_parms_1_7.e[1] = ctl_parms_1_7.e[0];
      
////      t = set_value_2_7 - result;
			ctl_parms_1_7.e[0] = set_value_1_7 - (int32_t)result;
		
			ctl_parms_1_7.d[0] += K1_PI_28_4*ctl_parms_1_7.e[0];
      
			ctl_parms_1_7.d[0] += K2_PI_28_4*ctl_parms_1_7.e[1];
			
      
      if (ctl_parms_1_7.d[0] > D_MAX) {
        ctl_parms_1_7.d[0] = D_MAX;
      } 
			else if (ctl_parms_1_7.d[0] < D_MIN) {
        ctl_parms_1_7.d[0] = D_MIN;
      } 
			
			TPM1->CONTROLS[1].CnV = ctl_parms_1_7.d[0]>>16;
			
			break;
			
		default:
			break;
	}
			

	ADC_channel++;
	if(ADC_channel>0)
      ADC_channel = 0;


//	PTC->PCOR |= GPIO_PCOR_PTCO(16);  //clear PTC16
	PTC->PCOR |= MASK(16);
////	PTD->PCOR = MASK(BLUE_LED_POS);


}


/**********Incremental Control******/	
////		error = 32767-result;
////	
////	if (error>0)
////		d++;
////	else
////		d--;
////	
////	if(d>920)
////	{
////		d=920;
////	}
////	if(d<20)
////	{
////		d=20;
////	}
/***********************************/


////////////////void ADC0_IRQHandler(){
////////////////////	NVIC_ClearPendingIRQ(ADC0_IRQn); 
////////////////////	PTD->PSOR = MASK(BLUE_LED_POS);
////////////////		
////////////////	    /* Start user code. Do not edit comment generated here */
////////////////#if FXP
////////////////    int16_t t;
////////////////#else
////////////////    volatile float error; // int16_t
////////////////#endif

////////////////      PTC->PCOR |= MASK(17);
////////////////			PTC->PSOR |= MASK(16);  //set PTC 16

////////////////			result= (int32_t)ADC0->R[0];
////////////////	
////////////////      ctl_parms_2_7.e[1] = ctl_parms_2_7.e[0];
////////////////      
////////////////      t = set_value_3_3 - result;  
////////////////		
////////////////      
////////////////      ctl_parms_2_7.e[0] = ((int32_t) t * (int32_t)27034) >> 16 ;
////////////////      
////////////////      
//////////////////            //burst mode
//////////////////      if(ctl_parms_2_7.e[0]<275 && ctl_parms_2_7.e[0]>-275)
//////////////////        break;

////////////////      
////////////////			D_local += K1_PI_28_4*ctl_parms_2_7.e[0];
////////////////      
////////////////////      // MAC new error and k1
////////////////////      __mach(K1_PI_12_4, ctl_parms_2_7.e[0]);
////////////////////      

////////////////			D_local += K2_PI_28_4*ctl_parms_2_7.e[1];
////////////////			
////////////////		
////////////////			ctl_parms_2_7.d[0] = D_local>>16;
////////////////      
////////////////      if (ctl_parms_2_7.d[0] > D_MAX) {
////////////////        ctl_parms_2_7.d[0] = D_MAX;
////////////////      } else if (ctl_parms_2_7.d[0] < D_MIN) {
////////////////        ctl_parms_2_7.d[0] = D_MIN;
////////////////      }       
////////////////			
////////////////			////		d = PIControl(result, ADC_channel);
////////////////			
////////////////			TPM0->CONTROLS[4].CnV=ctl_parms_2_7.d[0];
//////////////////	PTC->PCOR |= GPIO_PCOR_PTCO(16);  //clear PTC16
////////////////	PTC->PCOR |= MASK(16);
////////////////////	PTD->PCOR = MASK(BLUE_LED_POS);
////////////////}


