#include <MKL25Z4.H>
#include "user_defs.h"

#include "control_aj.h"
#include "pwm_aj.h"



FX_28_4 Kp_PI_28_4 = FL_TO_FX_28_4(0.175*PWM_PERIOD*SCALE);// Kp = - K2
FX_28_4 Ki_PI_28_4 = FL_TO_FX_28_4(0.0833*PWM_PERIOD*SCALE) + FL_TO_FX_28_4(-0.055*PWM_PERIOD*SCALE) ;// k1 = kp+ki
FX_28_4 K1_PI_28_4 = FL_TO_FX_28_4(0.175*PWM_PERIOD*SCALE) + FL_TO_FX_28_4(-0.055*PWM_PERIOD*SCALE) + FL_TO_FX_28_4(0.0833*PWM_PERIOD*SCALE); 
FX_28_4 K2_PI_28_4 = -FL_TO_FX_28_4(0.175*PWM_PERIOD*SCALE);// Kp = - K2


FX_28_4 K1_PI_MCU = FL_TO_FX_28_4(0.175*PWM_PERIOD) + FL_TO_FX_28_4(-0.055*PWM_PERIOD) + FL_TO_FX_28_4(0.0833*PWM_PERIOD); 
FX_28_4 K2_PI_MCU = -FL_TO_FX_28_4(0.175*PWM_PERIOD);// Kp = - K2

//////extern  FX_2_14 K1_PI_2_14;
//////extern  FX_2_14 K2_PI_2_14;


int32_t set_value_2_7 = SET_2_7; //  2.7 V in 4_12 format
int32_t set_value_3_3 = SET_3_3; //  3.3 V in 4_12 format
int32_t set_value_1_7 = SET_1_7; //  1.7 V in 4_12 format

int16_t pot_val=0, buck_out_val=0;

////////////uint32_t D_local = 3276800; //D_MIN<<16;


CTL_PARMS_T ctl_parms_3_3 =  {{0,0},{0,0}};
CTL_PARMS_T ctl_parms_2_7 =  {{0,0},{0,0}};
CTL_PARMS_T ctl_parms_1_7 =  {{0,0},{0,0}};

FX_20_12 e_2_7 = 0;
FX_20_12 e_2_7_prev = 0;


uint16_t PIControl(uint16_t adc_result, uint8_t ADC_channel)
{
////////////////////    /* Start user code. Do not edit comment generated here */
////////////////////#if FXP
////////////////////    int16_t t;
////////////////////#else
////////////////////    volatile float error; // int16_t
////////////////////#endif

////////////////////      
////////////////////      ctl_parms_2_7.e[1] = ctl_parms_2_7.e[0];
////////////////////      
////////////////////      t = set_value_3_3 - adc_result;  
////////////////////		
////////////////////      
////////////////////      ctl_parms_2_7.e[0] = (int16_t) (( (int32_t) t * 27034) >> 16) ;
////////////////////      
////////////////////      
//////////////////////            //burst mode
//////////////////////      if(ctl_parms_2_7.e[0]<275 && ctl_parms_2_7.e[0]>-275)
//////////////////////        break;

////////////////////      
////////////////////			D_local += K1_PI_28_4*ctl_parms_2_7.e[0];
////////////////////      
////////////////////////      // MAC new error and k1
////////////////////////      __mach(K1_PI_12_4, ctl_parms_2_7.e[0]);
////////////////////////      

////////////////////			D_local += K2_PI_28_4*ctl_parms_2_7.e[1];
////////////////////			
////////////////////////      // MAC old error and k2
////////////////////////      __mach(K2_PI_12_4, ctl_parms_2_7.e[1]);
////////////////////      
////////////////////////      ctl_parms_2_7.d[0] = MACRH;
////////////////////			
////////////////////			ctl_parms_2_7.d[0] = D_local>>16;
////////////////////      
////////////////////      if (ctl_parms_2_7.d[0] > D_MAX) {
////////////////////        ctl_parms_2_7.d[0] = D_MAX;
////////////////////      } else if (ctl_parms_2_7.d[0] < D_MIN) {
////////////////////        ctl_parms_2_7.d[0] = D_MIN;
////////////////////      }       
////////////////////      

////////////////////    return ctl_parms_2_7.d[0];//D_local;
////////////////////////    ADC_channel++;
////////////////////////    if(ADC_channel>7)
////////////////////////      ADC_channel = 4;

		return 0;
}







