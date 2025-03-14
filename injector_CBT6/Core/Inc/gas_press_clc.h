#ifndef INC_GAS_PRESS_CLC_H_
#define INC_GAS_PRESS_CLC_H_
#include "adc.h"
#include "usart.h"

extern float GasPressure;
extern uint8_t gaspres_refreshed;
extern uint16_t gaspres_adc_val;
extern uint8_t GasPrs_LOW;
extern uint8_t GasPrs_HIGH;

float GasPrs_CLC(uint16_t adc_val);
void refresh_gasPrs();

#endif /* INC_GAS_PRESS_CLC_H_ */
