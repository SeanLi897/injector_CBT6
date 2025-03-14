
#ifndef INC_AD24C02_H_
#define INC_AD24C02_H_

#include "gpio.h"
#include "i2c_soft.h"
#include "dosage.h"

#define DOSAGE_SET_ADDRESS 0x10
#define SUM_DOSAGE_ADDRESS 0x20
#define TOTAL_TREE_ADDRESS 0x30
#define CURRENT_TREE_NUMBER 0x40
#define CSV_LAST_LINE_ADDRESS 0x50

void AD24C02_Write2Byte(uint8_t memAddress, uint16_t *data);
void AD24C02_Read2Byte(uint8_t memAddress, uint16_t *data);
void READOUT_SAVE_DATA(void);
void AD24C02_DataCheck(void);

#endif /* INC_AD24C02_H_ */
