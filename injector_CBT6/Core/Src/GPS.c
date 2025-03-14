/* Private includes ----------------------------------------------------------*/
#include "gps.h"
/* Private variables ---------------------------------------------------------*/

uint8_t gps_rx_buffer[GPS_RX_BUFFER_SIZE];
volatile uint8_t GGA_data_used = 1;
volatile uint8_t RMC_data_used = 1;
volatile uint16_t gps_rx_index = 0;
volatile bool gps_data_ready = false;
volatile GPS_Data GPS_GGA_Data;
volatile GPS_Data GPS_RMC_Data;

/* Private function prototypes -----------------------------------------------*/


/* USER CODE BEGIN 0 */
// 重定向printf到UART1
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE {
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}



/* 解析GPS-GGA数据 */
int process_gps_data(const char *data, GPS_Data* result) {
    if (!data || !result) return -1;

    // 清空输出缓冲区
    memset(result, 0, sizeof(GPS_Data));

    // 校验和计算
    char *checksum_ptr = strchr(data, '*');
    if (!checksum_ptr) return -1; // 无校验和

    // 提取校验和
    uint8_t checksum_recv = strtol(checksum_ptr + 1, NULL, 16);
    uint8_t checksum_calc = 0;

    // 计算校验和（从$后到*前）
    for (const char *p = data + 1; p < checksum_ptr; p++) {
        checksum_calc ^= *p;
    }

    if (checksum_calc != checksum_recv) {
        return -2; // 校验和错误
    }

    // 解析协议头
    if (strncmp(data + 1, "GNGGA", 5) != 0 && strncmp(data + 1, "GNRMC", 5) != 0) {
        return -3; // 不支持的消息类型
    }
    strncpy(result->header, data + 1, 5);
    result->header[5] = '\0';

    // 分割字段
    char *token, *saveptr;
    char data_copy[128]; // 假设输入不超过128字节
    strncpy(data_copy, data, sizeof(data_copy));
    token = strtok_r(data_copy, ",", &saveptr);

    int field_index = 0;
    while (token) {
        switch (field_index) {
            case 1: // UTC时间（GGA/RMC）
                strncpy(result->time, token, sizeof(result->time) - 1);
                result->time[sizeof(result->time) - 1] = '\0';
                break;
            case 2: // 纬度（GGA）
								if (strcmp(result->header, "GNGGA") == 0){
									strncpy(result->latitude, token, sizeof(result->latitude) - 1);
									result->latitude[sizeof(result->latitude) - 1] = '\0';
								}
                break;
            case 3: // 纬度方向(GGA)/纬度(RMC)
            		if (strcmp(result->header, "GNGGA") == 0){
            				result->lat_dir = *token;
            		} else if (strcmp(result->header, "GNRMC") == 0) {
										strncpy(result->latitude, token, sizeof(result->latitude) - 1);
										result->latitude[sizeof(result->latitude) - 1] = '\0';
            		}
                break;
            case 4: // 经度(GGA)/纬度方向(RMC)
								if (strcmp(result->header, "GNGGA") == 0){
										strncpy(result->longitude, token, sizeof(result->longitude) - 1);
										result->longitude[sizeof(result->longitude) - 1] = '\0';
								}else if (strcmp(result->header, "GNRMC") == 0) {
										result->lat_dir = *token;
            		}
                break;
            case 5: // 经度方向(GGA)/经度(RMC)
								if (strcmp(result->header, "GNGGA") == 0){
										result->lon_dir = *token;
								}else if (strcmp(result->header, "GNRMC") == 0) {
										strncpy(result->longitude, token, sizeof(result->longitude) - 1);
										result->longitude[sizeof(result->longitude) - 1] = '\0';
            		}
                break;
            case 6: // 定位状态（GGA）/经度方向(RMC)
								if (strcmp(result->header, "GNGGA") == 0){
										result->fix_status = (uint8_t)atoi(token);
								}else if (strcmp(result->header, "GNRMC") == 0) {
										result->lon_dir = *token;
            		}
                break;
            case 9: // 海拔高度（GGA）或日期（RMC）
                if (strcmp(result->header, "GNGGA") == 0) {
                    result->altitude = atof(token);
                } else if (strcmp(result->header, "GNRMC") == 0) {
                    strncpy(result->date, token, sizeof(result->date) - 1);
                    result->date[sizeof(result->date) - 1] = '\0';
                }
                break;
        }
        token = strtok_r(NULL, ",", &saveptr);
        field_index++;
    }
    return 0; // 成功
}
//格式化日期字符串
void Format_GPS_Date(char* DateResault,char* time,char* date){
		char Resault[12] = {0};
		// 解析日期字符串
		char dd_part[3] = {0};
		char mm_part[3] = {0};
		char yy_part[3] = {0};

		strncpy(dd_part, date, 2);
		strncpy(mm_part, date + 2, 2);
		strncpy(yy_part, date + 4, 2);

		int dd = atoi(dd_part);
		int mm = atoi(mm_part);
		int yy = atoi(yy_part);

		// 解析时间字符串
		char hh_part[3] = {0};
		strncpy(hh_part, time, 2);
		int hh = atoi(hh_part);

		// 格式化输出
		snprintf(Resault,12,"%4d/%02d/%02d", 2000 + yy, mm, dd+((hh+8)/24));
		strncpy(DateResault, Resault, 15);
}

//格式化日期时间字符串
void Format_GPS_Time(char* TimeResault,char* time){
		char Resault[10] = {0};
		// 解析时间字符串
		char hh_part[3] = {0};
		char mi_part[3] = {0};
		char ss_part[3] = {0};

		strncpy(hh_part, time, 2);
		strncpy(mi_part, time + 2, 2);
		strncpy(ss_part, time + 4, 2);

		int hh = atoi(hh_part);
		int mi = atoi(mi_part);
		int ss = atoi(ss_part);

		// 格式化输出
		sprintf(Resault,"%02d:%02d:%02d", (hh+8)%24, mi, ss);
		strncpy(TimeResault, Resault, 12);
}

//格式化日期字符串(2025-02-22)
void Format_FileName_Date(char* NameResault,char* time,char* date){
//		char Resault[21] = {0};
		// 解析日期字符串
		char dd_part[3] = {0};
		char mm_part[3] = {0};
		char yy_part[3] = {0};

		strncpy(dd_part, date, 2);
		strncpy(mm_part, date + 2, 2);
		strncpy(yy_part, date + 4, 2);

		int dd = atoi(dd_part);
		int mm = atoi(mm_part);
		int yy = atoi(yy_part);

		// 解析时间字符串
		char hh_part[3] = {0};
		strncpy(hh_part, time, 2);
		int hh = atoi(hh_part);
		// 格式化输出
//		snprintf(NameResault,12,"%4d%02d%02d", 2000 + yy, mm, dd+((hh+8)/24));
		snprintf(NameResault,12,"%2d%02d%02d", yy, mm, dd+((hh+8)/24));
//		strncpy(NameResault, Resault, 8);
}

//格式化经度字符串
void FormatLongitude(char* LongitudeStr, char* Longitude, char Lon_dir) {
    // 解析输入字符串格式：DDDMM.MMMMM
    char dd_part[4] = {0};      // 度（3位）
    char mm_part[3] = {0};      // 分（2位）
    char decimal_part[6] = {0}; // 小数分（最多5位）

    // 分割字符串
    strncpy(dd_part, Longitude, 3);
    strncpy(mm_part, Longitude + 3, 2);

    // 查找小数点位置
    char* dot_ptr = strchr(Longitude, '.');
    if (dot_ptr) {
        strncpy(decimal_part, dot_ptr + 1, 5); // 取小数点后最多5位
    }

    // 转换为数值
    int dd = atoi(dd_part);
    int mm = atoi(mm_part);
//    double decimal_minutes = atof(decimal_part) / 100000.0; // 转换为小数分

    // 将小数分转换为秒（包含三位小数）
//    double total_seconds = decimal_minutes * 60.0;
//    int ss = (int)total_seconds;
//    int decimals = (int)((total_seconds - ss) * 1000.0 + 0.5); // 四舍五入到三位小数

    // 格式化输出到缓冲区（最大长度示例：E123°45'67.890）
//    snprintf(LongitudeStr, 20, "%c%03d°%02d’%02d.%03d\"", Lon_dir, dd, mm, ss, decimals);
    snprintf(LongitudeStr, 20, "%c%d°%02d’", Lon_dir, dd, mm);
}

//格式化纬度字符串
void FormatLatitude(char* LatitudeStr, char* Latitude, char Lat_dir) {
    // 解析输入字符串格式：DDMM.MMMMM
    char dd_part[3] = {0};      // 度（2位）
    char mm_part[3] = {0};      // 分（2位）
    char decimal_part[6] = {0}; // 小数分（最多5位）

    // 分割字符串
    strncpy(dd_part, Latitude, 2);
    strncpy(mm_part, Latitude + 2, 2);

    // 查找小数点位置
    char* dot_ptr = strchr(Latitude, '.');
    if (dot_ptr) {
        strncpy(decimal_part, dot_ptr + 1, 5); // 取小数点后最多5位
    }

    // 转换为数值
    int dd = atoi(dd_part);
    int mm = atoi(mm_part);
//    double decimal_minutes = atof(decimal_part) / 100000.0; // 转换为小数分

    // 将小数分转换为秒（包含三位小数）
//    double total_seconds = decimal_minutes * 60.0;
//    int ss = (int)total_seconds;
//    int decimals = (int)((total_seconds - ss) * 1000.0 + 0.5); // 四舍五入到三位小数

    // 格式化输出到缓冲区（最大长度示例：N23°45'67.890）
//    snprintf(LatitudeStr, 20, "%c%02d°%02d’%02d.%03d\"", Lat_dir, dd, mm, ss, decimals);
    snprintf(LatitudeStr, 20, "%c%d°%02d’", Lat_dir, dd, mm);
}

//海拔数据格式化（-9999.9~99999.9）
void FormatAltitude(char* AltitudeStr, float Altitude){
	snprintf(AltitudeStr, 7, "%.1f", Altitude);
}

int HMI_GPSDataRefresh(GPS_Data GGA_Result_Data, GPS_Data RMC_Result_Data){
	char Str_buf[30] = {0};
	char Tx_Buffer[50] = {0};

	if(GGA_data_used && RMC_data_used){
		return -1;
	}

	if(GGA_data_used == 0){
			//经度
			FormatLongitude(Str_buf, GGA_Result_Data.longitude, GGA_Result_Data.lon_dir);
			if(GGA_Result_Data.fix_status == 1)
				snprintf(Tx_Buffer,35,"Main.t14.txt=\"%s\"\xff\xff\xff",Str_buf);
			else
				snprintf(Tx_Buffer,35,"Main.t14.txt=\"E119°06'\"\xff\xff\xff");
			USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

			memset(Str_buf, '\0', sizeof(Str_buf));
			memset(Tx_Buffer, '\0', sizeof(Tx_Buffer));

			//纬度
			FormatLatitude(Str_buf, GGA_Result_Data.latitude, GGA_Result_Data.lat_dir);
			if(GGA_Result_Data.fix_status == 1)
				snprintf(Tx_Buffer,35,"Main.t15.txt=\"%s\"\xff\xff\xff",Str_buf);
			else
				snprintf(Tx_Buffer,35,"Main.t15.txt=\"N33°36'\"\xff\xff\xff");
			USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

			memset(Str_buf, '\0', sizeof(Str_buf));
			memset(Tx_Buffer, '\0', sizeof(Tx_Buffer));

			//海拔
			FormatAltitude(Str_buf, GGA_Result_Data.altitude);
			if(GGA_Result_Data.fix_status == 1)
				snprintf(Tx_Buffer,30,"Main.t17.txt=\"%s\"\xff\xff\xff",Str_buf);
			else
				snprintf(Tx_Buffer,30,"Main.t17.txt=\"28.0\"\xff\xff\xff");
			USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

			memset(Str_buf, '\0', sizeof(Str_buf));
			memset(Tx_Buffer, '\0', sizeof(Tx_Buffer));

			GGA_data_used = 1;
	}

	//日期时间
	if(RMC_data_used == 0){
		char date_buf[12] = {0};
		char time_buf[12] = {0};

			Format_GPS_Date(date_buf,RMC_Result_Data.time,RMC_Result_Data.date);
			Format_GPS_Time(time_buf,RMC_Result_Data.time);
			if(GGA_Result_Data.fix_status == 1)
				snprintf(Tx_Buffer,45,"Main.t4.txt=\"%s  %s\"\xff\xff\xff",date_buf, time_buf);
			else
				snprintf(Tx_Buffer,45,"Main.t4.txt=\"2025/01/01  12:00:00\"\xff\xff\xff");
			USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);

			memset(Str_buf, '\0', sizeof(Str_buf));
			memset(Tx_Buffer, '\0', sizeof(Tx_Buffer));

			RMC_data_used = 1;
	}
	return 0;
}

void Get_GPS_Date(char* DateStr, GPS_Data RMC_Result_Data){
	char Str_buf[10] = {0};
	if(RMC_data_used == 0){
		Format_GPS_Time(Str_buf,RMC_Result_Data.time);
		strncpy(DateStr,Str_buf,8);
		DateStr[8] = '\0';
	}
}

/* USER CODE END 0 */
