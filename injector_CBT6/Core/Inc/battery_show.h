#ifndef INC_BATTERY_SHOW_H_
#define INC_BATTERY_SHOW_H_
#include "usart.h"
#include "string.h"

extern uint8_t low_power;
extern uint16_t bat_adc_val;
extern uint8_t bat_val_refreshed;

void refresh_bat_vlt();

#endif /* INC_BATTERY_SHOW_H_ */
