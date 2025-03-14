/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "key.h"
#include "motor.h"
#include "adc.h"
#include "HMI.h"
#include "dosage.h"
//#include "GPS.h"

/* USER CODE END Includes */

extern TIM_HandleTypeDef htim3;

extern TIM_HandleTypeDef htim4;

/* USER CODE BEGIN Private defines */
extern volatile uint16_t key_press_time;
extern volatile uint32_t clear_delay_time;
extern volatile uint32_t cancel_delay_time;
extern volatile uint32_t ExGas_delay_time;
extern uint8_t EX_GAS_start;
extern uint8_t waiting_start_timeout;
extern uint16_t confirm_press_time;

//extern uint8_t File_manage_state;
//extern uint8_t Main_page_state;

extern uint8_t gaspres_refreshed;
extern volatile uint32_t now_time_inject;
extern volatile uint32_t last_time_inject;
extern volatile uint16_t GasPrs_LOW_time;
extern uint8_t cheat_flag;//作弊
extern uint8_t Invalid_action_times;
extern uint8_t over_pressure;

extern volatile uint32_t now_time;
extern volatile uint32_t confirm_time;
extern uint8_t RemainingTimes;//剩余注射次数
extern uint16_t total_inject_Times;//累计注药次数
extern uint16_t total_inject_Dosage;//累计注药量
extern uint16_t total_Times;

extern void Refresh_counter();
extern void Refresh_Dosage();
/* USER CODE END Private defines */

void MX_TIM3_Init(void);
void MX_TIM4_Init(void);

/* USER CODE BEGIN Prototypes */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

