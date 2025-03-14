#ifndef INC_SD_APP_H_
#define INC_SD_APP_H_

#include "main.h"
#include "SPI_SD.h"
#include "ff.h"
#include "usart.h"
#include "gpio.h"
#include "AD24C02.h"
#include "HMI.h"
#include "GPS.h"

#define DISK_SD "0:"
#define SD_INSERT 			(HAL_GPIO_ReadPin(TF_CD_GPIO_Port, TF_CD_Pin) == GPIO_PIN_RESET)

typedef struct {
	uint16_t No;
	char Date[11];
	char Time[10];
	uint16_t TreeSN;
	uint16_t TreeDose;
	char Lng[10];
	char Lat[10];
	char Alt[8];
	uint16_t CumDose;
	uint16_t CumQuty;
}SD_LogData;

extern int SDCard_states;
extern uint16_t csv_crt_line_No;
uint8_t SD_FS_Init(void);
uint8_t demo_user_fs(void);
int CSV_sheet_Init(void);
FRESULT check_file_exists(const char* path);

#endif /* INC_SD_APP_H_ */
