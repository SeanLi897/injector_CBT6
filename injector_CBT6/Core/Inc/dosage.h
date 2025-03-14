#ifndef INC_DOSAGE_H_
#define INC_DOSAGE_H_
#include "string.h"
#include "usart.h"
#include "gpio.h"
#include "AD24C02.h"
#include "key.h"

extern uint16_t Dosage_set;
extern uint16_t Dosage_load;
extern uint16_t current_TreeNo;

//extern uint8_t Injecting;
extern uint16_t Dosage_val;
extern uint8_t write_i2c;
extern uint8_t read_i2c;

extern uint8_t first_run_clcDos;

//uint8_t waiting_start_timeout;

void Set_Dosage(void);
void Refresh_counter();


#endif /* INC_DOSAGE_H_ */
