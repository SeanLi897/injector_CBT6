#include "gas_press_clc.h"

float GasPressure = 0.0;
uint8_t gaspres_refreshed = 0;
uint16_t gaspres_adc_val = 0;
uint8_t GasPrs_LOW = 0;
uint8_t GasPrs_HIGH = 0;

float GasPrs_CLC(uint16_t adc_val){
	float GasPrs_val = 0.0;
	GasPrs_val = (float)adc_val/4095.0*VREF*102.0/51.0*2.5;
	return GasPrs_val;
}

void refresh_gasPrs(){
	GasPressure = GasPrs_CLC(gaspres_adc_val);

	if(GasPressure < 0.08f){
		GasPrs_LOW = 1;
	}else{
		GasPrs_LOW = 0;
	}

	if(GasPressure > 8.0f){
		GasPrs_HIGH = 1;
	}else{
		GasPrs_HIGH = 0;
	}

	sprintf(Tx_Buffer,"Main.t3.txt=\"%.2f\"\xff\xff\xff",GasPressure);
	USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
}

