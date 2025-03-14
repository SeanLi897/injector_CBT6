/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "motor.h"
#include "string.h"
#include "gps.h"

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart2;

extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */
extern char Tx_Buffer[50];

extern volatile uint8_t Tx1_cplt_flag;
extern volatile uint8_t uart1_idle_flag;  // 空闲中断标志
extern volatile uint16_t uart1_received_len;  // 接收到的数据长度

extern volatile uint8_t Tx2_cplt_flag;
extern volatile uint8_t uart2_idle_flag;  // 空闲中断标志
extern volatile uint16_t uart2_received_len;  // 接收到的数据长度

extern volatile uint8_t Tx3_cplt_flag;
extern volatile uint8_t uart3_idle_flag;  // 空闲中断标志
extern volatile uint16_t uart3_received_len;  // 接收到的数据长度

extern uint8_t Tx_cplt_flag;
extern uint8_t rx1_buffer[RX_BUFFER_SIZE];
extern uint8_t rx2_buffer[RX_BUFFER_SIZE];
extern uint8_t rx3_buffer[RX_BUFFER_SIZE];
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void USART1_Tx_HMIdata(uint8_t *Tx_Buffer);
void USART1_Tx_data(uint8_t *Tx_Buffer);
void USART2_Tx_BLEdata(uint8_t *Tx_Buffer);
void USART3_Tx_GPSdata(uint8_t *Tx_Buffer);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

