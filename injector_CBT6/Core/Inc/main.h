/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef enum __bool { false = 0, true = 1, } bool;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define LOW_PWR_TEST 1
#ifndef BDC1_CRT_CHL
#define BDC1_CRT_CHL ADC_CHANNEL_0
#define BDC2_CRT_CHL ADC_CHANNEL_1
#define BAT_VLT_CHL  ADC_CHANNEL_2
#define GASP_VLT_CHL ADC_CHANNEL_3
#define RX_BUFFER_SIZE 256  // 接收缓冲区大小
#define GPS_RX_BUFFER_SIZE 256
#define VREF 3.31f
#endif
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DECREASE_Pin GPIO_PIN_13
#define DECREASE_GPIO_Port GPIOC
#define PAUSE_Pin GPIO_PIN_14
#define PAUSE_GPIO_Port GPIOC
#define LED_R_Pin GPIO_PIN_15
#define LED_R_GPIO_Port GPIOC
#define TF_CD_Pin GPIO_PIN_0
#define TF_CD_GPIO_Port GPIOD
#define LED_G_Pin GPIO_PIN_1
#define LED_G_GPIO_Port GPIOD
#define GPS_PPS_Pin GPIO_PIN_0
#define GPS_PPS_GPIO_Port GPIOA
#define ADC_GASP_Pin GPIO_PIN_1
#define ADC_GASP_GPIO_Port GPIOA
#define TF_CS_Pin GPIO_PIN_4
#define TF_CS_GPIO_Port GPIOA
#define VADC_BDC1_Pin GPIO_PIN_0
#define VADC_BDC1_GPIO_Port GPIOB
#define ADC_BAT_Pin GPIO_PIN_1
#define ADC_BAT_GPIO_Port GPIOB
#define BLE_LED_Pin GPIO_PIN_2
#define BLE_LED_GPIO_Port GPIOB
#define BDC1_STP_Pin GPIO_PIN_12
#define BDC1_STP_GPIO_Port GPIOB
#define NRESET_Pin GPIO_PIN_13
#define NRESET_GPIO_Port GPIOB
#define PH1_Pin GPIO_PIN_14
#define PH1_GPIO_Port GPIOB
#define BDC_EN2_Pin GPIO_PIN_15
#define BDC_EN2_GPIO_Port GPIOB
#define BDC_EN1_Pin GPIO_PIN_8
#define BDC_EN1_GPIO_Port GPIOA
#define KEY_EXTI11_Pin GPIO_PIN_11
#define KEY_EXTI11_GPIO_Port GPIOA
#define KEY_EXTI11_EXTI_IRQn EXTI15_10_IRQn
#define Q_SET_Pin GPIO_PIN_12
#define Q_SET_GPIO_Port GPIOA
#define START_Pin GPIO_PIN_15
#define START_GPIO_Port GPIOA
#define INCREASE_Pin GPIO_PIN_3
#define INCREASE_GPIO_Port GPIOB
#define LED_Y_Pin GPIO_PIN_4
#define LED_Y_GPIO_Port GPIOB
#define CONFIRM_Pin GPIO_PIN_5
#define CONFIRM_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB
#define EX_GAS_Pin GPIO_PIN_8
#define EX_GAS_GPIO_Port GPIOB
#define BUZZER_Pin GPIO_PIN_9
#define BUZZER_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
void Refresh_counter();
void Refresh_Dosage();
float GasPrs_CLC(uint16_t adc_val);
float Motor_CRT_CLC();
void refresh_bat_vlt();
void refresh_gasPrs();

extern uint16_t ADC_rslt[4];
extern uint16_t motor1_adc_val;
extern uint16_t motor2_adc_val;
extern uint16_t bat_adc_val;
extern uint16_t gaspres_adc_val;
extern uint8_t ADC_CONV_cplt;
extern uint8_t GasPrs_LOW;


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
