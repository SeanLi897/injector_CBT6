#ifndef INC_BLUETOOTH_H_
#define INC_BLUETOOTH_H_
#include "usart.h"

typedef enum {
	NOT_CONNECTED,
	BLE_CONNECTED,
	BLE_ERR,
} BLE_Status;
extern BLE_Status BLE_conn_sta;
extern uint8_t BLE_conn_sta;
#endif /* INC_BLUETOOTH_H_ */
