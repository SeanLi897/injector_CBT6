/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
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
/* Includes ------------------------------------------------------------------*/
#include "tim.h"

/* USER CODE BEGIN 0 */
volatile uint32_t now_time_inject = 0;
volatile uint32_t last_time_inject = 0;
volatile uint16_t GasPrs_LOW_time = 0;

uint16_t confirm_press_time = 0;
uint8_t over_pressure = 0;
uint8_t cheat_flag = 0;
uint8_t Invalid_action_times = 0;
volatile uint32_t confirm_time = 0;
uint8_t RemainingTimes = 0;//剩余注射次数
uint16_t total_inject_Times = 0;//累计注药次数
uint16_t total_inject_Dosage = 0;//累计注药量
uint16_t total_Times = 0;
uint8_t waiting_start_timeout = 0;
uint8_t EX_GAS_start = 0;

volatile uint32_t key_last_time = 0,key_now_time = 0;
volatile uint32_t now_time = 0;
volatile uint32_t clear_delay_time = 0;
volatile uint32_t cancel_delay_time = 0;
volatile uint32_t ExGas_delay_time = 0;
/* USER CODE END 0 */

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

/* TIM3 init function */
void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 6400-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}
/* TIM4 init function */
void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 64-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 1000-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* TIM3 clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();

    /* TIM3 interrupt Init */
    HAL_NVIC_SetPriority(TIM3_IRQn, 13, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
  /* USER CODE BEGIN TIM3_MspInit 1 */

  /* USER CODE END TIM3_MspInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspInit 0 */

  /* USER CODE END TIM4_MspInit 0 */
    /* TIM4 clock enable */
    __HAL_RCC_TIM4_CLK_ENABLE();

    /* TIM4 interrupt Init */
    HAL_NVIC_SetPriority(TIM4_IRQn, 13, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
  /* USER CODE BEGIN TIM4_MspInit 1 */

  /* USER CODE END TIM4_MspInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspDeInit 0 */

  /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();

    /* TIM3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
  /* USER CODE BEGIN TIM3_MspDeInit 1 */

  /* USER CODE END TIM3_MspDeInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspDeInit 0 */

  /* USER CODE END TIM4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM4_CLK_DISABLE();

    /* TIM4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM4_IRQn);
  /* USER CODE BEGIN TIM4_MspDeInit 1 */

  /* USER CODE END TIM4_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//Timer3 重装载中断间�???100ms
//Timer4 重装载中断间�???1ms
//Timer3、Timer4 共用同一回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim == &htim3){
		now_time_inject++;

		if((now_time_inject % 10 == 0) && ADC_CONV_cplt){
			ADC_CONV_cplt = 0;
			if(page_location == Main_page){
				refresh_bat_vlt();
				refresh_gasPrs();
				HMI_GPSDataRefresh(GPS_GGA_Data,GPS_RMC_Data);
			}
		}

		if(Injecting){
			if(cheat_flag == 1){
				sprintf(Tx_Buffer,"Main.t0.txt=\"请按规\r\n范操作\"\xff\xff\xff");
				USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

				if(now_time_inject % 5 == 0){
					HAL_GPIO_TogglePin(LED_Y_GPIO_Port, LED_Y_Pin);
				}

				pause_state = 1;

				InjectTimes += Invalid_action_times;
				RemainingTimes += Invalid_action_times;

				if(total_inject_Times > Invalid_action_times){
					total_inject_Times -= Invalid_action_times;
				}
				else{
					total_inject_Times = 0;
				}

				if(total_inject_Dosage > (Invalid_action_times)*2){
					total_inject_Dosage =total_inject_Dosage - (Invalid_action_times)*2 ;
				}
				else {
					total_inject_Dosage = 0;
				}
				Invalid_action_times  = 0;

				sprintf(Tx_Buffer,"Main.n0.val=%d\xff\xff\xff",RemainingTimes*2);//RemainingDosage
				USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

				Refresh_counter();

			}else if(cheat_flag == 0 || cancel_break){

				HAL_GPIO_WritePin(LED_Y_GPIO_Port, LED_Y_Pin, GPIO_PIN_RESET);
			}
		}

		if(Injecting && !pause_state){
			if(now_time_inject % 5 == 0)
				HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin);
			}
			else
			{
				if(clear_delay_time >= 3000){
					total_inject_Times = 0;
					total_Times = 0;
					total_inject_Dosage = 0;
					current_TreeNo = 0;
					clear_counter = 1;
					clear_delay_time = 0;

					Buzzer(1000);
					Refresh_counter();

					HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin,GPIO_PIN_RESET);
				}
			}

		if(cancel_delay_time >= 3000 && (page_location == Main_page)){
			if(Injecting && pause_state){
				cancel_delay_time = 0;
				Injecting = 0;
				totalCycles = 0;
				InjectTimes = 0;
				RemainingTimes = 0;
				cancel_break = 1;
				cheat_flag = 0;
				current_TreeNo--;

				sprintf(Tx_Buffer,"Main.t0.txt=\"取消中...\"\xff\xff\xff");
				USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

				sprintf(Tx_Buffer,"Main.n0.val=0\xff\xff\xff");
				USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

				sprintf(Tx_Buffer,"Main.t8.txt=\"\"\xff\xff\xff");
				USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

				Motor_Reverse();
			}
			else if(waiting_start){
				waiting_start = 0;
				cancel_delay_time = 0;
				Dosage_load = 0;
				Dosage_set = 0;

				sprintf(Tx_Buffer,"Main.n0.val=%d\xff\xff\xff",Dosage_set);
				USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

				sprintf(Tx_Buffer,"Main.t8.txt=\"\"\xff\xff\xff");
				USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
			}
			Dosage_set = 0;
			sprintf(Tx_Buffer,"Main.t0.txt=\"\"\xff\xff\xff");
			USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
		}

		if(ExGas_delay_time >= 3500 && (page_location == Main_page)){
			EX_GAS_start = 1;
		}
		else
		if(ExGas_delay_time >= 3500 && (page_location == File_M_page)){
				ExGas_delay_time = 0;
				EX_GAS_start = 0;
			}

		if(Injecting && over_pressure){
			sprintf(Tx_Buffer,"Main.t0.txt=\"注射压\r\n力超压\"\xff\xff\xff");
			USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

			if(now_time_inject % 5 == 0){
				HAL_GPIO_TogglePin(LED_Y_GPIO_Port, LED_Y_Pin);
			}
			pause_state = 1;
		}

		if(confirm_press_time >= 3000 && (page_location == Main_page)){
			confirm_press_time = 0;
			page_location = File_M_page;
			sprintf(Tx_Buffer,"page File_M\xff\xff\xff");
			USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
			HAL_Delay(20);
			refresh_dir = 1;
		}

	}//&htim3


	if(htim == &htim4){
		now_time++;

		if((key_lock_time - now_time >= 50) && key_scan_lock){//下降沿中断触发后50ms内锁止，防止反复触发
			key_scan_lock = 0;
			key_lock_time = 0;
		}

		if(key_pressed)
			key_now_time++;

		if(GasPrs_LOW == 1){

			if(motor_direction == FORWARD){
				GasPrs_LOW_time++;
			}

		}else if(GasPrs_LOW == 0)
		{
			GasPrs_LOW_time = 0;
		}
//单板调试时关闭
		if(GasPrs_LOW_time >= 4500){
			cheat_flag = 1;
		}
		else if(GasPrs_LOW_time == 0){
			cheat_flag = 0;
		}

		if(GasPrs_HIGH == 1){
			over_pressure = 1;
		}

		if((PAUSE_KEY == GPIO_PIN_SET) || (NRESET_KEY == GPIO_PIN_SET)){
    	HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin,GPIO_PIN_RESET);
    	clear_delay_time = 0;
		}
		else if((clear_counter == 0) && (PAUSE_KEY == GPIO_PIN_RESET) && (NRESET_KEY == GPIO_PIN_RESET)){
    	HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin,GPIO_PIN_SET);
    	clear_delay_time++;
		}

		if(PAUSE_KEY == GPIO_PIN_SET){
			cancel_delay_time = 0;
		}
		else if(PAUSE_KEY == GPIO_PIN_RESET){
			cancel_delay_time++;
		}

		if(EX_GAS_KEY == GPIO_PIN_SET){
			ExGas_delay_time = 0;
		}
		else if(EX_GAS_start != 1 && EX_GAS_KEY == GPIO_PIN_RESET){
			ExGas_delay_time++;
		}

		if(CONFIRM_KEY == GPIO_PIN_SET){
			confirm_press_time = 0;
//			HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);
		}
		else if(Injecting != 1 && CONFIRM_KEY == GPIO_PIN_RESET){
			confirm_press_time++;
//			HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET);
		}

		//按键被按下且超过1.5s，标记为长按按键
			if(key_pressed && key_value && (key_now_time - key_last_time > 1200) && !key_press_long){
				key_now_time = 0;
				key_last_time = 0;
				key_press_long = 1;
			}

		if(key_press_long && (page_location == Main_page)){
			key_press_time++;

			if(key_press_time >= 500){
				if(key_value == DECREASE_KEY_PRESS){
				  if(Dosage_set > 1000)
					  Dosage_set = 0;
				  else if(Dosage_set >= 2)
					  Dosage_set-=2;
					key_value_Refresh = 1;
				}
				else if(key_value == INCREASE_KEY_PRESS)
				{
				  if(Dosage_set < 1000)
					  Dosage_set+=2;
				  else if(Dosage_set >= 1000)
					  Dosage_set = 1000;
					key_value_Refresh = 1;
				}
				key_press_time = 0;
			}//key_press_time
		}//key_press_long


		//确认键被按下，开始计时，5秒内waiting_start标识没消失判定等待开始超时
		if(waiting_start){
			if((now_time > confirm_time) && (now_time - confirm_time > 5000)){
				waiting_start = 0;
				Dosage_load = 0;
				waiting_start_timeout = 1;
			}
			else if((now_time <= confirm_time) && (0xffff - confirm_time + now_time> 5000)){
				waiting_start = 0;
				Dosage_load = 0;
				waiting_start_timeout = 1;
			}
		}

	}//&htim4
}
/* USER CODE END 1 */
