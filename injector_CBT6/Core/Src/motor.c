#include "motor.h"

uint8_t motor_position = 0;
int8_t motor_direction = 0;
uint8_t current_motor = 0;
uint8_t first_run = 1;
uint8_t InjectTimes= 0; // 电机1�?工作次数
uint8_t Injecting = 0;
uint8_t totalCycles = 0; // 总工作过程次数
uint8_t pause_state = 0;
uint16_t motor1_adc_val = 0;
uint16_t motor2_adc_val = 0;
float motor1_crt = 0.0;


//电机初始化，电机反转，回到TOP位置
void Motor_init(){
	Motor_Reverse();
	HAL_Delay(1800);
	Motor_Brake();
}

void Motor_Forward(){
	motor_direction = FORWARD;

	Motor_1_PH_SET;
	Motor_1_EN_SET;
}

void Motor_Reverse(){
	motor_direction = REVERSE;

	Motor_1_EN_SET;
	Motor_1_PH_RESET;
}

void Motor_Brake(){
	motor_direction = 0;

	Motor_1_EN_RESET;
	Motor_1_PH_RESET;
}

void Ex_GAS_Cycle(){

	motor_Forward_Cycle();
	while(EX_GAS_start){
		Motor_2_EN_SET;
	}
	Motor_2_EN_RESET;
	motor_Reverse_Cycle();
}

void PAUSE_chk(){
	uint8_t pause_show = 0;
	static uint8_t enter_pause = 0;
	while(Injecting && pause_state){
		enter_pause = 1;
		HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET);
		if((now_time_inject % 5 == 0) && !pause_show){
			if(cheat_flag == 1){
				sprintf(Tx_Buffer,"Main.t0.txt=\"请按规\r\n范操作\"\xff\xff\xff");
				USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
			}
			else if(over_pressure == 1){
				sprintf(Tx_Buffer,"Main.t0.txt=\"注射压\r\n力超压\"\xff\xff\xff");
				USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
			}
			else
			{
				sprintf(Tx_Buffer,"Main.t0.txt=\"已暂停\"\xff\xff\xff");
				USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
			}
			pause_show = !pause_show;
		}
		else if((now_time_inject % 5 == 0) && pause_show){
			sprintf(Tx_Buffer,"Main.t0.txt=\"\"\xff\xff\xff");
			USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
			pause_show = !pause_show;
		}
		//长按暂停按键清除累计值
		else if(clear_counter){
			clear_counter = 0;
			Refresh_counter();
		}
	}

	if(enter_pause){
			sprintf(Tx_Buffer,"Main.t0.txt=\"\"\xff\xff\xff");
			USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
			enter_pause = 0;
	}
}

void motor_Forward_Cycle(){
	Motor_Forward();
	last_time_inject = now_time_inject;

//	while(now_time_inject - last_time_inject <= 15){//推杆泵前推1.5s
//		if(BDC1_STP_STATE == GPIO_PIN_SET){
//				Motor_Brake();
//				break;
//		}
//	}
	//调试用
	while(now_time_inject - last_time_inject <= 15);//推杆泵前推1.5s
//	HAL_Delay(1500);
				Motor_Brake();
}

void motor_Reverse_Cycle(){
	Motor_Reverse();
	last_time_inject = now_time_inject;
	while(now_time_inject - last_time_inject <= 15);//推杆泵收缩1.5s
//	HAL_Delay(1500);
	Motor_Brake();
}

void Inject_times(uint16_t Dosage_load){
	totalCycles = Dosage_load / 2;
	Dosage_load = 0;
	RemainingTimes = totalCycles;
	InjectTimes = totalCycles;
}

void Pump_work(){
	Motor_2_EN_SET;
	last_time_inject = now_time_inject;
	while(now_time_inject - last_time_inject <= 6);
//	HAL_Delay(800);
	Motor_2_EN_RESET;
}

void Inject_working(void){
	uint8_t i =0;
	Invalid_action_times = 0;
	GasPrs_LOW_time = 0;
	for(i = 0;i < InjectTimes;i++){
		motor_Forward_Cycle();				//推杆泵前推1.5s

		RemainingTimes--;

		total_inject_Times++;
		total_inject_Dosage += 2;

		Refresh_Dosage();

		if(GasPrs_LOW){
			Invalid_action_times++;
		}
		else{
			Invalid_action_times = 0;
		}
		motor_Reverse_Cycle();       //推杆泵收缩1.5s
		HAL_Delay(1000);//停顿1s			 //停顿1s
		Pump_work();								 //泵工作0.8s

		PAUSE_chk();
	}

	if(clear_counter){
			clear_counter = 0;
		}

	if(cancel_break){
			cancel_break = 0;
		}
	else{
			total_Times++;
		}

	sprintf(Tx_Buffer,"Main.t0.txt=\"注药结束\"\xff\xff\xff");
	USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);


	sprintf(Tx_Buffer,"Main.t8.txt=\"\"\xff\xff\xff");
	USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

	Refresh_counter();

	SDCard_Write_log(GPS_GGA_Data,GPS_RMC_Data);
	refresh_dir = 1;

	Buzzer(1000);

	Injecting = 0;
	first_run = 1;
	HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET);
}

float Motor_CRT_CLC(){
	float motor_crt = 0.0;
	motor_crt = motor1_adc_val/4096.0*VREF/1690.0/450;
	return motor_crt;
}
