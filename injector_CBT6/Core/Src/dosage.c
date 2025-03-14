#include "dosage.h"
uint16_t Dosage_val = 0;
uint8_t first_run_clcDos = 0;
uint8_t write_i2c = 0;
uint8_t read_i2c = 0;
uint16_t Dosage_set;
uint16_t Dosage_load;
uint16_t current_TreeNo = 0;

void Set_Dosage(){
	//刷新注药量设定值
		if(key_value_Refresh){
			sprintf(Tx_Buffer,"Main.n0.val=%d\xff\xff\xff",Dosage_set);
			USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
			key_value_Refresh = 0;

			sprintf(Tx_Buffer,"Main.t0.txt=\"\"\xff\xff\xff");
			USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
		}
	//确认键按下，等待启动按键按下
		if(confirm_pressed && waiting_start){
			confirm_pressed = 0;
		}
	//等待START状态下START按键被按下，显示√
		if(waiting_start && start_pressed){
			waiting_start = 0;
			start_pressed = 0;
			Injecting = 1;
			first_run_clcDos = 1;
			current_TreeNo++;

			AD24C02_Write2Byte(DOSAGE_SET_ADDRESS,&Dosage_load);
			HAL_Delay(10);

			sprintf(Tx_Buffer,"Main.t8.txt=\"√\"\xff\xff\xff");
			USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

			sprintf(Tx_Buffer,"Main.t0.txt=\"\"\xff\xff\xff");
			USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

			sprintf(Tx_Buffer,"Main.t19.txt=\"%04d\"\xff\xff\xff",current_TreeNo);
			USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
		}
	//等待START时间超时，√消失，需要重新按下确认键
		if(waiting_start_timeout){
			waiting_start_timeout = 0;
			waiting_start = 0;

			sprintf(Tx_Buffer,"Main.t8.txt=\"\"\xff\xff\xff");
			USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
		}

		//Q_SET按键按下，读取上次保存的剂量值
		if(read_i2c && !waiting_start && !Injecting){
			read_i2c = 0;
			AD24C02_Read2Byte(DOSAGE_SET_ADDRESS,&Dosage_val);
			Dosage_set = Dosage_val;
			sprintf(Tx_Buffer,"Main.n0.val=%d\xff\xff\xff",Dosage_set);
			USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
		}
}

void Refresh_counter(){
	AD24C02_Write2Byte(SUM_DOSAGE_ADDRESS,&total_inject_Dosage);
	HAL_Delay(10);
	AD24C02_Write2Byte(TOTAL_TREE_ADDRESS,&total_Times);
	HAL_Delay(10);
	AD24C02_Write2Byte(CURRENT_TREE_NUMBER,&current_TreeNo);
	HAL_Delay(10);

	sprintf(Tx_Buffer,"Main.n2.val=%d\xff\xff\xff",total_Times);
	USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

	sprintf(Tx_Buffer,"Main.n3.val=%d\xff\xff\xff",total_inject_Dosage);
	USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

	sprintf(Tx_Buffer,"Main.t19.txt=\"%04d\"\xff\xff\xff",current_TreeNo);
	USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
}

void Refresh_Dosage(){
	uint16_t RemainingDosage = 0;
	RemainingDosage = RemainingTimes * 2;

	if(Injecting){
		sprintf(Tx_Buffer,"Main.n0.val=%d\xff\xff\xff",RemainingDosage);
		USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

		sprintf(Tx_Buffer,"Main.t0.txt=\"正在注药\"\xff\xff\xff");
		USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

		Refresh_counter();
	}
}
