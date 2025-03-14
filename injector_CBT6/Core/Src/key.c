#include "key.h"
uint8_t key_scan_lock = 0;
uint32_t key_lock_time = 0;
uint8_t waiting_start = 0;
uint8_t confirm_pressed = 0;
uint8_t start_pressed = 0;
uint8_t nReset_pressed = 0;
uint8_t pause_pressed = 0;
uint8_t clear_counter = 0;
uint8_t cancel_break = 0;
uint8_t EX_GAS_pressed = 0;
uint8_t del_file_pressed = 0;//
uint8_t confirm_del_pressed = 0;
uint8_t key_value = 0;
uint8_t key_value_Refresh = 0;
uint8_t key_press_long = 0;
uint8_t key_press_sigle = 0;
uint8_t key_pressed = 0;
volatile uint16_t key_press_time = 0;

volatile enum KEY_VAL key_code;

//在按键中断中调用
uint8_t key_scan(){

	if(KEY_EXTI11 == GPIO_PIN_RESET){
		HAL_Delay(20);
		if(KEY_EXTI11 == GPIO_PIN_RESET){
			if(!DECREASE_KEY) 			return DECREASE_KEY_PRESS;
			else if(!CONFIRM_KEY) 	return CONFIRM_KEY_PRESS;
			else if(!INCREASE_KEY) 	return INCREASE_KEY_PRESS;
			else if(!Q_SET_KEY) 		return Q_SET_KEY_PRESS;
			else if(!EX_GAS_KEY) 		return EX_GAS_KEY_PRESS;
			else if(!START_KEY) 		return START_KEY_PRESS;
			else if(!NRESET_KEY) 		return NRESET_KEY_PRESS;
			else if(!PAUSE_KEY) 		return PAUSE_KEY_PRESS;
		}
	}
	return 0;
}

void deal_key(){
		if(key_value && key_press_sigle){
		key_press_sigle = 0;//清除单次按标记，防止重复执行

		switch(key_value){//检测按键值
			case DECREASE_KEY_PRESS://减小
			{
			if(page_location == Main_page){
				if(!waiting_start && !Injecting){//不处于已按确认等待按开始键状态
					if(Dosage_set > 1000)
						Dosage_set = 0;
					else if(Dosage_set > 0)
						Dosage_set -= 2;
					key_value_Refresh = 1;
				}
			}
			else
			if(page_location == File_M_page){//光标下移
				last_focus_line = current_focus_line;
				if(current_focus_line > 0){
					current_focus_line--;
				}else if(current_focus_line == 0){
					current_focus_line = 5;
				}
				dir_display_refresh = 1;
			}
			}
			break;
			case CONFIRM_KEY_PRESS://确认
			{
				if(page_location == Main_page){
					if(!Injecting && (Dosage_set != 0)){
						Dosage_load = Dosage_set;
						confirm_time = now_time;
						confirm_pressed = 1;
						waiting_start = 1;
					}

					if(Injecting && cheat_flag){
						Dosage_load = RemainingTimes*2;
						confirm_time = now_time;
						confirm_pressed = 1;
						waiting_start = 1;
					}

					if(Injecting && over_pressure){
						confirm_time = now_time;
						confirm_pressed = 1;
						waiting_start = 1;
					}
				}
				else
				if(page_location == File_M_page){//确认、翻页
					focus_key_pressed = 1;
					key_code = KEY_CONFIRM;
				}
			}
			break;
			case INCREASE_KEY_PRESS://增加
			{
			if(page_location == Main_page){
				if(!waiting_start && !Injecting){
					if(Dosage_set < 1000)
						Dosage_set+=2;
					else if(Dosage_set >= 1000)
						Dosage_set = 1000;
					key_value_Refresh = 1;
				}
			}
			else
			if(page_location == File_M_page){//光标上移
				last_focus_line = current_focus_line;
				if(current_focus_line < 5){
					current_focus_line++;
				}else if(current_focus_line == 5){
					current_focus_line = 0;
				}
				dir_display_refresh = 1;
			}
			}
			break;
			case Q_SET_KEY_PRESS://快设
			{
			if(page_location == Main_page){
				read_i2c = 1;
			}
			else
			if(page_location == File_M_page){//删除文件
				key_code = KEY_DELETE;
			}
			}
			break;
			case EX_GAS_KEY_PRESS://排气
			{
			if(page_location == Main_page){
				EX_GAS_pressed = 1;
			}
			else
			if(page_location == File_M_page){//发送文件
				key_code = KEY_SENDFILE;
			}
			}
			break;
			case START_KEY_PRESS://开始
			{
			if(page_location == Main_page){
				if(waiting_start && (Dosage_load > 0) && !Injecting){
					start_pressed = 1;
					Dosage_set = 0;
				}
				else{
					start_pressed = 0;
				}

				if(cheat_flag){
					cheat_flag = 0;
					GasPrs_LOW_time = 0;
					GasPrs_LOW = 0;
					pause_state = 0;
				}

				if(over_pressure){
					over_pressure = 0;
					GasPrs_HIGH = 0;
					pause_state = 0;
				}
			}
			else
			if(page_location == File_M_page){
				key_code = KEY_SENDFILE;
			}
			}
			break;
			case NRESET_KEY_PRESS://复位键
			{
				if(page_location == Main_page){
					nReset_pressed = 1;
				}
				else
				if(page_location == File_M_page){
					key_code = KEY_CANCEL;
				}
			}
			break;
			case PAUSE_KEY_PRESS://暂停
			{
			if(page_location == Main_page){
				pause_pressed = 1;
				if(Injecting && !pause_state){
					pause_state = 1;
				}
				else if(Injecting && pause_state && !cheat_flag){
					pause_state = 0;
				}
			}
			else
			if(page_location == File_M_page){

			}
			}
			break;
		}
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (HAL_GPIO_ReadPin(KEY_EXTI11_GPIO_Port, KEY_EXTI11_Pin) == GPIO_PIN_SET) {
	// 处理上升沿中断
		HAL_GPIO_WritePin(LED_Y_GPIO_Port, LED_Y_Pin,GPIO_PIN_RESET);
		key_pressed = 0;
		key_value = 0;
		key_last_time = 0;
		key_now_time = 0;
		key_value_Refresh = 0;
		key_press_time = 0;
		key_press_long = 0;

		if(HAL_GPIO_ReadPin(NRESET_GPIO_Port,NRESET_Pin) == GPIO_PIN_SET){
			nReset_pressed = 0;
		}

		if(HAL_GPIO_ReadPin(PAUSE_GPIO_Port,PAUSE_Pin) == GPIO_PIN_SET){
			pause_pressed = 0;
		}

		if(HAL_GPIO_ReadPin(EX_GAS_GPIO_Port,EX_GAS_Pin) == GPIO_PIN_SET){
			EX_GAS_start = 0;
			EX_GAS_pressed = 0;
		}
	}

	if (HAL_GPIO_ReadPin(KEY_EXTI11_GPIO_Port, KEY_EXTI11_Pin) == GPIO_PIN_RESET) {
	// 处理下降沿中断
		if(!key_scan_lock){
			key_scan_lock = 1;//防止多次执行下降沿中断函数
			key_lock_time = now_time;

			HAL_GPIO_WritePin(LED_Y_GPIO_Port, LED_Y_Pin,GPIO_PIN_SET);
			key_pressed = 1;
			key_value = key_scan();
			key_last_time = key_now_time;
			key_press_sigle = 1;

			deal_key();
			HAL_GPIO_WritePin(LED_Y_GPIO_Port, LED_Y_Pin,GPIO_PIN_RESET);
		}
	}
}
