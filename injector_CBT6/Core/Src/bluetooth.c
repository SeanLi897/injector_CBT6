#include "bluetooth.h"

BLE_Status check_ble_status(void){
	HAL_UART_Transmit(&huart2, (uint8_t *)"AT+ENAT\r\n", 9, 50);

	while(1){
		if(HAL_UART_Receive(&huart2, rx2_buffer, 4, 50) == HAL_OK){
			if(strstr(rx2_buffer,(uint8_t *)"OK") != NULL){
//				printf("Enter AT mode success!\r\n");
//				printf("AT+ENAT receive:%s\r\n",rx2_buffer);
				break;
			}
		}else{
//			printf("Enter AT mode failed!\r\n");
//			printf("AT+ENAT receive:%s\r\n",rx2_buffer);
			return BLE_ERR;
		}
	}

	HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CONN\r\n", 9, 50);
	while(1){
		if(HAL_UART_Receive(&huart2, rx2_buffer, 3, 50) == HAL_OK){
			if(rx2_buffer[0] == '1'){
//				printf("AT+CONN receive:%s\r\n",rx2_buffer);
				BLE_conn_sta = 1;
				break;
			}
		}else{
//			sprintf(Tx_Buffer, "File_M.t7.txt=\"蓝牙未连接\"\xff\xff\xff");
//			printf("AT+CONN receive:%s\r\n",rx2_buffer);
//			USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
 			return NOT_CONNECTED;
		}
	}

	HAL_UART_Transmit(&huart2, (uint8_t *)"AT+EXAT\r\n", 9, 50);

	while(1){
		if(HAL_UART_Receive(&huart2, rx2_buffer, 4, 50) == HAL_OK){
			if(strstr(rx2_buffer,(uint8_t *)"OK") != NULL){
//				printf("Exit AT mode success!\r\n");
//				printf("AT+EXAT receive:%s\r\n",rx2_buffer);
				break;
			}
		}else{
//			printf("Exit AT mode failed!\r\n");
//			printf("AT+EXAT receive:%s\r\n",rx2_buffer);
			return BLE_ERR;
		}
	}

	return BLE_CONNECTED;
}

