#include "SD_APP.h"

// 增加写入超时
#define SD_WRITE_TIMEOUT 10000  // 10秒
int SDCard_states = 0;
uint16_t csv_crt_line_No = 0;

FRESULT f_write_retry(FIL* fp, const void* buff, UINT btw) {
  UINT bw;
  FRESULT res;
  uint32_t start = HAL_GetTick();

  do {
    res = f_write(fp, buff, btw, &bw);

    if(res != FR_OK) break;
    if(HAL_GetTick() - start > SD_WRITE_TIMEOUT) {
      res = FR_TIMEOUT;
      break;
    }
  } while(bw != btw);

  return res;
}

int SDCard_InsertCheck(void){
	uint8_t SD_Check_retry = 0;
	int SDCard_states = 0;

  if(SD_INSERT){
  	int ret = 0;
  	do
		{
		ret = SD_FS_Init();
		}while(ret != 0 && SD_Check_retry++ >5);

  	if(ret == 0){
  		SDCard_states = 0; //正常
//  		printf("SDCard Init success!\r\n");
  	}
  	else{
  		SDCard_states = -1;//初始化失败
//  		printf("SDCard Init fail!\r\n");
  	}
  }
  else{
  	SDCard_states = -2;//未插入SD卡
//  	printf("SDCard is not insert!\r\n");
  }
  return SDCard_states;
}

uint8_t SD_FS_Init(void)
{
    static FATFS sd_fs;
    uint32_t ret = 0;
    MKFS_PARM opt = {0};
    opt.fmt = FM_FAT;
    static uint8_t work_buf[512] = {0};
    ret = f_mount(&sd_fs, DISK_SD, 1);

    if(ret != FR_OK)
    {
        switch(ret)
        {
            case FR_NO_FILESYSTEM:
            {
                ret = f_mkfs(DISK_SD, &opt, work_buf, sizeof(work_buf));
            }break;
            case FR_MKFS_ABORTED:
            {
                ret = f_mkfs(DISK_SD, &opt, work_buf, sizeof(work_buf));
            }break;
        }
    }
    if(ret != 0)
    {
//      printf("sd fs f_mkfs fail(%d)\r\n", ret);
    }
    else
    {
//    	printf("sd fs f_mkfs success!\r\n");
    }
    return ret;
}

FRESULT check_file_exists(const char* path) {
    FILINFO fno;
    return f_stat(path, &fno);
}

int CSV_sheet_Init(void){

	if(SDCard_states == 0){
    FIL fp;
    FRESULT res;
    uint16_t File_Num = 1;

    char USER_FILE_NAME[20] = {0};
    char file_name_buf[20] = {0};

    if(GPS_GGA_Data.fix_status == 1){
    	Format_FileName_Date(file_name_buf, GPS_RMC_Data.time, GPS_RMC_Data.date);
    }
    else{
    	sprintf(file_name_buf,"250101");
    }
    snprintf(USER_FILE_NAME,20,"0:/%s%02d.csv", file_name_buf, File_Num);

    res = check_file_exists(USER_FILE_NAME);

    if (res == FR_OK) {//文件存在
    	AD24C02_Read2Byte(CSV_LAST_LINE_ADDRESS, &csv_crt_line_No);
    	return 0;
    }
    else if (res == FR_NO_FILE) {//文件不存在
        FRESULT ret = f_open(&fp, USER_FILE_NAME, FA_WRITE | FA_OPEN_APPEND);
        if(ret != FR_OK)
        {
//        	printf("f_open fail: %s\r\n", USER_FILE_NAME);
          return 1;
        }

    		char header[] = {0xE5,0xBA,0x8F,0xE5,0x8F,0xB7,0x2C,0xE6,0x97,0xA5,0xE6
    				,0x9C,0x9F,0x2C,0xE6,0x97,0xB6,0xE9,0x97,0xB4,0x2C,0xE6,0xA0,0x91
						,0xE6,0x9C,0xA8,0xE7,0xBC,0x96,0xE5,0x8F,0xB7,0x2C,0xE6,0xB3,0xA8
						,0xE5,0xB0,0x84,0xE9,0x87,0x8F,0x2F,0x6D,0x4C,0x2C,0xE7,0xBB,0x8F
						,0xE5,0xBA,0xA6,0x2C,0xE7,0xBA,0xAC,0xE5,0xBA,0xA6,0x2C,0xE6,0xB5
						,0xB7,0xE6,0x8B,0x94,0x2F,0x6D,0x2C,0xE7,0xB4,0xAF,0xE8,0xAE,0xA1
						,0xE6,0xB3,0xA8,0xE8,0x8D,0xAF,0xE9,0x87,0x8F,0x2F,0x6D,0x4C,0x2C
						,0xE7,0xB4,0xAF,0xE8,0xAE,0xA1,0xE6,0xB3,0xA8,0xE8,0x8D,0xAF,0xE6,0x95,0xB0,0x0D,0X0A};
        ret = f_write_retry(&fp, header, strlen(header));

        f_sync(&fp);

        if(ret != FR_OK)
        {
//        	printf("f_open fail:%d\r\n", ret );
          return 1;
        }

        ret = f_close(&fp);
        if(ret != FR_OK)
        {
//        	printf("f_close fail:%d\r\n", ret);
          return 1;
        }

        csv_crt_line_No = 0;
        AD24C02_Write2Byte(CSV_LAST_LINE_ADDRESS, &csv_crt_line_No);
        return 0;

    } else {
//        printf("错误码: %d\r\n", res);  // 其他错误（如硬件问题）
        return 1;
    }
	}
	else
	{
		return -1;//没发现SDCard
	}
}

int SDCard_Write_log(GPS_Data GGA_Result_Data, GPS_Data RMC_Result_Data){
	if(SDCard_states == 0){
    FIL fp;
    FRESULT res;
    uint16_t File_Num = 1;
    char USER_FILE_NAME[20] = {0};
    char file_name_buf[20] = {0};
    SD_LogData LogBuf;

    if(GGA_Result_Data.fix_status == 1){    //按日期格式化文件名
    	Format_FileName_Date(file_name_buf, GPS_RMC_Data.time, GPS_RMC_Data.date);
    }
    else{
    	sprintf(file_name_buf,"250101");
    }
    snprintf(USER_FILE_NAME,20,"0:/%s%02d.csv", file_name_buf, File_Num);

    res = check_file_exists(USER_FILE_NAME);    //检查文件是否存在
    FRESULT ret = f_open(&fp, USER_FILE_NAME, FA_WRITE | FA_OPEN_APPEND);    //打开文件，若文件不存在则创建新文件，并定位到文件末尾
    if(ret != FR_OK)
    {
//    	printf("f_open fail: %s\r\n", USER_FILE_NAME);
      return 1;
    }

    switch(res){
    case FR_NO_FILE://文件不存在，先向文件中写入列名称：序号,日期,时间,树木编号,注射剂量,经度,纬度,海拔,累计剂量,累计数量
    		char header[] = {0xE5,0xBA,0x8F,0xE5,0x8F,0xB7,0x2C,0xE6,0x97,0xA5,0xE6,0x9C,0x9F,
    				0x2C,0xE6,0x97,0xB6,0xE9,0x97,0xB4,0x2C,0xE6,0xA0,0x91,0xE6,0x9C,0xA8,0xE7,
						0xBC,0x96,0xE5,0x8F,0xB7,0x2C,0xE6,0xB3,0xA8,0xE5,0xB0,0x84,0xE9,0x87,0x8F,
						0x2F,0x6D,0x4C,0x2C,0xE7,0xBB,0x8F,0xE5,0xBA,0xA6,0x2C,0xE7,0xBA,0xAC,0xE5,
						0xBA,0xA6,0x2C,0xE6,0xB5,0xB7,0xE6,0x8B,0x94,0x2F,0x6D,0x2C,0xE7,0xB4,0xAF,
						0xE8,0xAE,0xA1,0xE6,0xB3,0xA8,0xE8,0x8D,0xAF,0xE9,0x87,0x8F,0x2F,0x6D,0x4C,
						0x2C,0xE7,0xB4,0xAF,0xE8,0xAE,0xA1,0xE6,0xB3,0xA8,0xE8,0x8D,0xAF,0xE6,0x95,0xB0,0x0D,0X0A};
        ret = f_write_retry(&fp, header, strlen(header));
        f_sync(&fp);
        csv_crt_line_No = 0;
        AD24C02_Write2Byte(CSV_LAST_LINE_ADDRESS, &csv_crt_line_No);
    case FR_OK://文件存在
    		char write_bf[50];
				LogBuf.No = csv_crt_line_No++;
				Format_GPS_Date(LogBuf.Date,RMC_Result_Data.time,RMC_Result_Data.date);
				Format_GPS_Time(LogBuf.Time,RMC_Result_Data.time);
				LogBuf.TreeSN = current_TreeNo;
				LogBuf.TreeDose = InjectTimes*2;
				FormatLongitude(LogBuf.Lng, GGA_Result_Data.longitude, GGA_Result_Data.lon_dir);
				FormatLatitude(LogBuf.Lat, GGA_Result_Data.latitude, GGA_Result_Data.lat_dir);
				FormatAltitude(LogBuf.Alt, GGA_Result_Data.altitude);
				LogBuf.CumDose = total_inject_Dosage;
				LogBuf.CumQuty = total_Times;

				AD24C02_Write2Byte(CSV_LAST_LINE_ADDRESS, &csv_crt_line_No);
//				printf("\r\n");

//				printf("%04d,",LogBuf.No);//第1列：序号 0001
				sprintf(write_bf,"%4d,",LogBuf.No);
				ret = f_write_retry(&fp, write_bf, strlen(write_bf));
				if (ret != FR_OK) {
//				    printf("Write failed: %d\n", ret);
				    f_close(&fp);
				    return 1;
				}
//				memset(write_bf,'\0',sizeof(write_bf));

				if(GGA_Result_Data.fix_status == 1){
//					printf("%s,",LogBuf.Date);//第2列：日期 2025/02/25
					sprintf(write_bf,"%s,",LogBuf.Date);//第2列：日期 2025/02/25
				}
				else{
//					printf("2025/01/01,");//第2列：日期 2025/01/01
					sprintf(write_bf,"2025/01/01,");//第2列：日期 2025/01/01
				}
				ret = f_write_retry(&fp, write_bf, strlen(write_bf));
				if (ret != FR_OK) {
//				    printf("Write failed: %d\n", ret);
				    f_close(&fp);
				    return 1;
				}
//				memset(write_bf,'\0',sizeof(write_bf));

				if(GGA_Result_Data.fix_status == 1){
//					printf("%s,",LogBuf.Time);//第3列：时间 18:21:21
					sprintf(write_bf,"%s,",LogBuf.Time);//第3列：时间 18:21:21
				}
				else{
//					printf("12:00:00,");//第3列：时间 12:00:00
					sprintf(write_bf,"12:00:00,");//第3列：时间 12:00:00
				}
				ret = f_write_retry(&fp, write_bf, strlen(write_bf));
				if (ret != FR_OK) {
//				    printf("Write failed: %d\n", ret);
				    f_close(&fp);
				    return 1;
				}
//				memset(write_bf,'\0',sizeof(write_bf));

//				printf("%04d,",LogBuf.TreeSN);//第4列：序号 0001
				sprintf(write_bf,"%4d,",LogBuf.TreeSN);
				ret = f_write_retry(&fp, write_bf, strlen(write_bf));
				if (ret != FR_OK) {
//				    printf("Write failed: %d\n", ret);
				    f_close(&fp);
				    return 1;
				}
//				memset(write_bf,'\0',sizeof(write_bf));

//				printf("%d,",LogBuf.TreeDose);//第5列：注射剂量 30
				sprintf(write_bf,"%d,",LogBuf.TreeDose);
				ret = f_write_retry(&fp, write_bf, strlen(write_bf));
				if (ret != FR_OK) {
//				    printf("Write failed: %d\n", ret);
				    f_close(&fp);
				    return 1;
				}
//				memset(write_bf,'\0',sizeof(write_bf));

				if(GGA_Result_Data.fix_status == 1){
//					printf("%s,",LogBuf.Lng);//第6列：经度 E118°48'
					sprintf(write_bf,"%s,",LogBuf.Lng);//第6列：经度 E118°48'
				}
				else{
//					printf("E118°48',");//第6列：经度 E118°48'
					sprintf(write_bf,"E119°06',");//第6列：经度 E118°48
				}
				ret = f_write_retry(&fp, write_bf, strlen(write_bf));
				if (ret != FR_OK) {
//				    printf("Write failed: %d\n", ret);
				    f_close(&fp);
				    return 1;
				}
//				memset(write_bf,'\0',sizeof(write_bf));

				if(GGA_Result_Data.fix_status == 1){
//					printf("%s,",LogBuf.Lat);//第7列：纬度 N31°50'
					sprintf(write_bf,"%s,",LogBuf.Lat);//第7列：纬度 N31°50'
				}
				else{
//					printf("N31°50',");//第7列：纬度 N31°50'
					sprintf(write_bf,"N33°36',");//第7列：纬度 N31°50'
				}
				ret = f_write_retry(&fp, write_bf, strlen(write_bf));
				if (ret != FR_OK) {
//				    printf("Write failed: %d\n", ret);
				    f_close(&fp);
				    return 1;
				}
//				memset(write_bf,'\0',sizeof(write_bf));

				if(GGA_Result_Data.fix_status == 1){
//					printf("%s,",LogBuf.Alt);//第8列：海拔 88.9
					sprintf(write_bf,"%s,",LogBuf.Alt);//第8列：海拔 88.9
				}
				else{
//					printf("88.9,");//第8列：海拔 88.9
					sprintf(write_bf,"28.0,");//第8列：海拔 88.9
				}
				ret = f_write_retry(&fp, write_bf, strlen(write_bf));
				if (ret != FR_OK) {
//				    printf("Write failed: %d\n", ret);
				    f_close(&fp);
				    return 1;
				}
//				memset(write_bf,'\0',sizeof(write_bf));

//				printf("%d,",LogBuf.CumDose);//第9列，累计剂量 892
				sprintf(write_bf,"%d,",LogBuf.CumDose);
				ret = f_write_retry(&fp, write_bf, strlen(write_bf));
				if (ret != FR_OK) {
//				    printf("Write failed: %d\n", ret);
				    f_close(&fp);
				    return 1;
				}
//				memset(write_bf,'\0',sizeof(write_bf));

//				printf("%d\r\n",LogBuf.CumQuty);//第10列，累计数量 112
				sprintf(write_bf,"%d\r\n",LogBuf.CumQuty);
				ret = f_write_retry(&fp, write_bf, strlen(write_bf));
				if (ret != FR_OK) {
//				    printf("Write failed: %d\n", ret);
				    f_close(&fp);
				    return 1;
				}
				memset(write_bf,'\0',sizeof(write_bf));

				f_sync(&fp);

        ret = f_close(&fp);
        if(ret != FR_OK)
        {
//        	printf("f_close fail:%d\r\n", ret);
          return 1;
        }else{
//        	sprintf(Tx_Buffer,"Main.t0.txt=\"SDCard write success！\"\xff\xff\xff");
//        	USART1_Tx_HMIdata((uint8_t*)Tx_Buffer);
        }
				break;
    default:
//				printf("错误码: %d\r\n", res);  // 其他错误（如硬件问题）
				break;
    }

	}
	else
	{
		return -1;//没发现SDCard
	}
}


