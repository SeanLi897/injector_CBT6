#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "gpio.h"
#include "adc.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "key.h"
#include "tim.h"
#include "dosage.h"
#include "HMI.h"

#define Motor_1 1
#define Motor_2 2

#define FORWARD 1
#define REVERSE -1

#define Motor_1_EN_SET HAL_GPIO_WritePin(BDC_EN1_GPIO_Port,BDC_EN1_Pin,GPIO_PIN_SET)
#define Motor_1_PH_SET HAL_GPIO_WritePin(PH1_GPIO_Port,PH1_Pin,GPIO_PIN_SET)
#define Motor_2_EN_SET HAL_GPIO_WritePin(BDC_EN2_GPIO_Port,BDC_EN2_Pin,GPIO_PIN_SET)
#define Motor_2_PH_SET HAL_GPIO_WritePin(PH2_GPIO_Port,PH2_Pin,GPIO_PIN_SET)

#define Motor_1_EN_RESET HAL_GPIO_WritePin(BDC_EN1_GPIO_Port,BDC_EN1_Pin,GPIO_PIN_RESET)
#define Motor_1_PH_RESET HAL_GPIO_WritePin(PH1_GPIO_Port,PH1_Pin,GPIO_PIN_RESET)
#define Motor_2_EN_RESET HAL_GPIO_WritePin(BDC_EN2_GPIO_Port,BDC_EN2_Pin,GPIO_PIN_RESET)
#define Motor_2_PH_RESET HAL_GPIO_WritePin(PH2_GPIO_Port,PH2_Pin,GPIO_PIN_RESET)

#define BDC1_STP_STATE HAL_GPIO_ReadPin(BDC1_STP_GPIO_Port,BDC1_STP_Pin)
#define BDC2_STP_STATE HAL_GPIO_ReadPin(BDC2_STP_GPIO_Port,BDC2_STP_Pin)

extern uint8_t totalCycles; // 总工作过程次数
extern uint8_t InjectTimes; // 每个电机的工作次数
extern volatile uint32_t now_time_inject;
extern volatile uint32_t last_time_inject;
extern uint8_t EX_GAS_start;
extern uint8_t pause_state;
extern uint8_t motor_position;
extern int8_t motor_direction;
extern uint8_t current_motor;
extern uint8_t last_motor;
extern uint16_t motor1_adc_val;
extern uint16_t motor2_adc_val;
extern float motor1_crt;

void Motor_init();
void Motor_Forward();
void Motor_Reverse();
void Motor_Brake();
extern void USART1_Tx_HMIdata(uint8_t *Tx_Buffer);
extern void USART1_Tx_data(uint8_t *Tx_Buffer);
extern void Refresh_Dosage();
extern void Buzzer(uint16_t beep_ms);

extern uint8_t first_run;
extern uint8_t current_motor;
extern uint8_t Injecting;

void motor_Forward_Cycle();
void motor_Reverse_Cycle();
void Ex_GAS_Cycle();
void Inject_times(uint16_t Dosage_load);
void Inject_working();
float Motor_CRT_CLC();

#endif /* INC_MOTOR_H_ */
