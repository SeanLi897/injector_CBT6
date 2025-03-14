/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "key.h"
#include "motor.h"
#include "AD24C02.h"
#include "GPS.h"
#include "ff.h"
#include "SD_APP.h"
#include "battery_show.h"
#include "dosage.h"
#include "HMI.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

extern uint8_t _estack, _Min_Stack_Size;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  MX_TIM3_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)ADC_rslt, 4);
	__HAL_DMA_DISABLE_IT(&hdma_adc1, DMA_IT_HT);
  HAL_Delay(10);

  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_Base_Start_IT(&htim4);
  HAL_Delay(10);

  HMI_init();
	HAL_Delay(100);

  Motor_init();
  first_run_clcDos = 1;
  first_run = 1;

	READOUT_SAVE_DATA();
	AD24C02_DataCheck();
	HAL_Delay(10);

	page_location = Main_page;
  HAL_UART_Receive_DMA(&huart3, &gps_rx_buffer[0], 1); // 启动接收
  SDCard_states = SDCard_InsertCheck();
  if(SDCard_states == 0){
    CSV_sheet_Init();
  }else{
//  	printf("CSV file initialization failed!\r\n");
  }

  AD24C02_Read2Byte(CSV_LAST_LINE_ADDRESS, &csv_crt_line_No);//读取最后一次保存的行号
//  printf("System Initialized\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  	static uint16_t i = 0;
  	if(i++ >=10000){
			if(SDCard_states != 0 || !SD_INSERT){
				SDCard_states = SDCard_InsertCheck();
				HAL_GPIO_WritePin(LED_Y_GPIO_Port, LED_Y_Pin, GPIO_PIN_SET);
			}
			else if(SDCard_states == 0){
				HAL_GPIO_WritePin(LED_Y_GPIO_Port, LED_Y_Pin, GPIO_PIN_RESET);
			}
			i = 0;
  	}

  	if(Injecting && (page_location == Main_page)){
			sprintf(Tx_Buffer,"Main.t0.txt=\"正在注药\"\xff\xff\xff");
			USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

			if(first_run_clcDos == 1){
				Inject_times(Dosage_load);
				first_run_clcDos = 0;
			}
			Inject_working();
		}
		else
		if(!Injecting && EX_GAS_start && (page_location == Main_page)){
				while(!Injecting && EX_GAS_start)
					Ex_GAS_Cycle();
		}
		else
		if(!Injecting && (page_location == File_M_page)){
			if(refresh_dir || first_display_dir){
				Cache_File_List();
				Refresh_Display();
				scroll_focus_line();
				first_display_dir = 0;
				refresh_dir = 0;
			}

			switch(key_code){
				case KEY_CANCEL:
					Back_to_MainPage();
					break;
				case KEY_CONFIRM:
					page_turning();
					break;
				case KEY_DELETE:
					On_Delete_Key_Pressed();
					break;
				case KEY_SENDFILE:
					sendFile_key_pressed();
					break;
				default:
					break;
			}
			key_code = KEY_NULL;

			if(dir_display_refresh){
				scroll_focus_line();
				dir_display_refresh = 0;
			}
		}
		else
		if(page_location == Main_page)
		{
			Set_Dosage();
		}

		if(clear_counter && (page_location == Main_page)){
			clear_counter = 0;
			Refresh_Dosage();
			HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin,GPIO_PIN_SET);
			HAL_Delay(300);
			HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin,GPIO_PIN_RESET);
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV8;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
