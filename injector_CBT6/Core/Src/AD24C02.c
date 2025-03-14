#include "AD24C02.h"

#define AD24C02_ADDRESS 0XA0

void AD24C02_Write2Byte(uint8_t memAddress, uint16_t *data) {
    uint8_t buffer[3];
    buffer[0] = memAddress;          // Memory address
    buffer[1] = (uint8_t)(*data);    // Low byte of data
    buffer[2] = (uint8_t)(*data >> 8); // High byte of data
    I2C_WriteData(AD24C02_ADDRESS, buffer, 3); // Write address + 2 bytes
}

void AD24C02_Read2Byte(uint8_t memAddress, uint16_t *data) {
   I2C_WriteData(AD24C02_ADDRESS, &memAddress, 1);
   I2C_ReadData(AD24C02_ADDRESS, (uint8_t*)data, 2);
}

void READOUT_SAVE_DATA(void){
	AD24C02_Read2Byte(SUM_DOSAGE_ADDRESS,&total_inject_Dosage);
	HAL_Delay(10);
	AD24C02_Read2Byte(TOTAL_TREE_ADDRESS,&total_Times);
	HAL_Delay(10);
	AD24C02_Read2Byte(CURRENT_TREE_NUMBER,&current_TreeNo);
	HAL_Delay(10);

	sprintf(Tx_Buffer,"Main.n2.val=%d\xff\xff\xff",total_Times);
	USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

	sprintf(Tx_Buffer,"Main.n3.val=%d\xff\xff\xff",total_inject_Dosage);
	USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

	sprintf(Tx_Buffer,"Main.t19.txt=\"%04d\"\xff\xff\xff",current_TreeNo);
	USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
}

void AD24C02_DataCheck(void){
	if(total_inject_Dosage > 9999){
		total_inject_Dosage = 0;
		AD24C02_Write2Byte(SUM_DOSAGE_ADDRESS,&total_inject_Dosage);
	}
	if(total_Times > 9999){
		total_Times = 0;
		AD24C02_Write2Byte(TOTAL_TREE_ADDRESS,&total_Times);
	}
	if(current_TreeNo > 9999){
		current_TreeNo = 0;
		AD24C02_Write2Byte(CURRENT_TREE_NUMBER,&current_TreeNo);
	}
}
