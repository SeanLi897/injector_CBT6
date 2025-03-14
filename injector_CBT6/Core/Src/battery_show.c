#include "battery_show.h"

uint8_t low_power =0;
uint16_t bat_adc_val = 0;
uint8_t bat_val_refreshed = 0;

void refresh_bat_vlt(){
	uint16_t level_0 = 0;
	uint16_t level_20 = 0;
	uint16_t level_40 = 0;
	uint16_t level_60 = 0;
	uint16_t level_80 = 0;

	level_0 = (uint16_t)(19.2/20/VREF*4095);//19.2V
	level_20 = (uint16_t)(20.4/20/VREF*4095);//20.4V
	level_40 = (uint16_t)(21.0/20/VREF*4095);//21V
	level_60 = (uint16_t)(22.0/20/VREF*4095);//22V
	level_80 = (uint16_t)(24.0/20/VREF*4095);//24V

	bat_adc_val = (uint16_t)(bat_adc_val*1.01);
	if(bat_adc_val <= level_0){
		low_power = 1;

		//请立即充电，0%
		sprintf(Tx_Buffer,"Main.n4.val=0\xff\xff\xff");
		USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
		sprintf(Tx_Buffer,"Main.j0.val=0\xff\xff\xff");
		USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
		sprintf(Tx_Buffer,"Main.t0.txt=\"请立即\r\n 充电\"\xff\xff\xff");
		USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
	}else if(bat_adc_val > level_0 && bat_adc_val <= level_20){
		//请尽快充电,20%
		low_power = 0;

		sprintf(Tx_Buffer,"Main.n4.val=20\xff\xff\xff");
		USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
		sprintf(Tx_Buffer,"Main.j0.val=20\xff\xff\xff");
		USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
		sprintf(Tx_Buffer,"Main.t0.txt=\"请尽快\r\n 充电\"\xff\xff\xff");
		USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
	}else if(bat_adc_val > level_20 && bat_adc_val <= level_40){
		//40%
		low_power = 0;

		sprintf(Tx_Buffer,"Main.n4.val=40\xff\xff\xff");
		USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
		sprintf(Tx_Buffer,"Main.j0.val=40\xff\xff\xff");
		USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
	}else if(bat_adc_val > level_40 && bat_adc_val <= level_60){
		//60%
		low_power = 0;

		sprintf(Tx_Buffer,"Main.n4.val=60\xff\xff\xff");
		USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
		sprintf(Tx_Buffer,"Main.j0.val=60\xff\xff\xff");
		USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
	}else if(bat_adc_val > level_60 && bat_adc_val <= level_80){
		//80%
		low_power = 0;

		sprintf(Tx_Buffer,"Main.n4.val=80\xff\xff\xff");
		USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
		sprintf(Tx_Buffer,"Main.j0.val=80\xff\xff\xff");
		USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
	}else if(bat_adc_val > level_80){
		//100%
		low_power = 0;

		sprintf(Tx_Buffer,"Main.n4.val=100\xff\xff\xff");
		USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
		sprintf(Tx_Buffer,"Main.j0.val=100\xff\xff\xff");
		USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
	}

}
