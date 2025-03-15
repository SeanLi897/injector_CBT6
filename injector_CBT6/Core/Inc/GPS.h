#ifndef INC_GPS_H_
#define INC_GPS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include <stdlib.h>

/* 新增结构体 */
typedef struct {
	char year[5];
	char month[3];
	char day[3];
}DATE_TYPE;

typedef struct {
    char header[7];        // 协议头 (如"GNGGA")
    char time[12];         // UTC时间 hhmmss.mmm (包含毫秒)
    char latitude[12];     // 纬度
    char lat_dir;          // 纬度方向
    char longitude[12];    // 经度
    char lon_dir;          // 经度方向
    uint8_t fix_status;    // 定位状态
    uint8_t satel_Num;	   // 卫星数量
    float altitude;        // 海拔高度 (单位:米)
    DATE_TYPE today;
} GPS_Data;

int process_gps_data(const char *data, GPS_Data* result);
extern volatile uint8_t GGA_data_used;
extern volatile uint8_t ZDA_data_used;
extern uint8_t gps_rx_buffer[GPS_RX_BUFFER_SIZE];
extern volatile uint16_t gps_rx_index;
extern volatile bool gps_data_ready;
extern volatile GPS_Data GPS_GGA_Data;
extern volatile GPS_Data GPS_ZDA_Data;
extern volatile uint8_t GPS_time_get;
void Format_GPS_Date(char* DateResault, DATE_TYPE date);
void Format_GPS_Time(char* TimeResault,char* time);
int HMI_GPSDataRefresh(GPS_Data GGA_Result_Data, GPS_Data GPS_ZDA_Data);
void Format_FileName_Date(char* NameResault, DATE_TYPE date);

#endif /* INC_GPS_H_ */
