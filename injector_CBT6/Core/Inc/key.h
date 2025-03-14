#ifndef INC_KEY_H_
#define INC_KEY_H_
#include "gpio.h"
#include "usart.h"
#include "stdio.h"
#include "dosage.h"
#include "gas_press_clc.h"
#include "HMI.h"

extern uint8_t confirm_pressed;
extern uint8_t start_pressed;
extern uint8_t pause_pressed;
extern uint8_t nReset_pressed;
extern uint8_t EX_GAS_pressed;
extern uint8_t del_file_pressed;
extern uint8_t confirm_del_pressed;

enum KEY_VAL {KEY_NULL,KEY_CANCEL,KEY_CONFIRM,KEY_DELETE,KEY_SENDFILE};
extern volatile enum KEY_VAL key_code;

extern uint8_t clear_counter;
extern uint8_t cancel_break;

extern volatile uint32_t key_last_time,key_now_time;
extern uint8_t key_value;
extern uint8_t key_press_long;
extern uint8_t key_press_sigle;
extern uint8_t key_pressed;
extern uint8_t key_value_Refresh;
extern uint8_t key_scan_lock;
extern uint32_t key_lock_time;
extern uint8_t waiting_start;


//定义按键键值
#define DECREASE_KEY_PRESS 1
#define CONFIRM_KEY_PRESS 2
#define INCREASE_KEY_PRESS 3
#define Q_SET_KEY_PRESS 4
#define EX_GAS_KEY_PRESS 5
#define START_KEY_PRESS 6
#define NRESET_KEY_PRESS 7
#define PAUSE_KEY_PRESS 8

//读取按键IO电平
#define DECREASE_KEY HAL_GPIO_ReadPin(DECREASE_GPIO_Port, DECREASE_Pin)
#define CONFIRM_KEY HAL_GPIO_ReadPin(CONFIRM_GPIO_Port, CONFIRM_Pin)
#define INCREASE_KEY HAL_GPIO_ReadPin(INCREASE_GPIO_Port, INCREASE_Pin)
#define Q_SET_KEY HAL_GPIO_ReadPin(Q_SET_GPIO_Port, Q_SET_Pin)
#define EX_GAS_KEY HAL_GPIO_ReadPin(EX_GAS_GPIO_Port, EX_GAS_Pin)
#define START_KEY HAL_GPIO_ReadPin(START_GPIO_Port, START_Pin)
#define NRESET_KEY HAL_GPIO_ReadPin(NRESET_GPIO_Port, NRESET_Pin)
#define PAUSE_KEY HAL_GPIO_ReadPin(PAUSE_GPIO_Port, PAUSE_Pin)

//按键外部中断公共触发脚
#define KEY_EXTI11 HAL_GPIO_ReadPin(KEY_EXTI11_GPIO_Port, KEY_EXTI11_Pin)

void deal_key();
uint8_t key_scan();
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
#endif /* INC_KEY_H_ */
