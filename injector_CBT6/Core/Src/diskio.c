/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "SPI_SD.h"

/* Definitions of physical drive number for each drive */
#define DEV_SPI_SD		0	/* Example: Map Ramdisk to physical drive 0 */
#define SPI_FLASH     1     // 预留外部SPI Flash使用
//#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */
//#define DEV_SPI_SD 3 /* Example: Map SPI_SD to physical drive 3 */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
  DSTATUS status = STA_NOINIT;
  switch (pdrv) {
  case DEV_SPI_SD: /* SD CARD */
      status &= ~STA_NOINIT;
      break;

  case SPI_FLASH:        /* SPI Flash */
      break;

  default:
      status = STA_NOINIT;
  }
  return status;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
  DSTATUS status = STA_NOINIT;
  switch (pdrv) {
  case DEV_SPI_SD:          /* SD CARD */
      if (SD_Init()==SD_RESPONSE_NO_ERROR) {
          status &= ~STA_NOINIT;
      } else {
          status = STA_NOINIT;
      }

      break;

  case SPI_FLASH:    /* SPI Flash */
      break;

  default:
      status = STA_NOINIT;
  }
  return status;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
  DRESULT status = RES_PARERR;
  SD_Error SD_state = SD_RESPONSE_NO_ERROR;

  switch (pdrv) {
  case DEV_SPI_SD: /* SD CARD */
      SD_state=SD_ReadMultiBlocks(buff,sector *SD_BLOCK_SIZE,SD_BLOCK_SIZE,count);

      if (SD_state!=SD_RESPONSE_NO_ERROR)
          status = RES_PARERR;
      else
          status = RES_OK;
      break;

  case SPI_FLASH:
      break;

  default:
      status = RES_PARERR;
  }
  return status;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
  DRESULT status = RES_PARERR;
  SD_Error SD_state = SD_RESPONSE_NO_ERROR;

  if (!count) {
      return RES_PARERR;    /* Check parameter */
  }

  switch (pdrv) {
  case DEV_SPI_SD: /* SD CARD */

      SD_state=SD_WriteMultiBlocks((uint8_t *)buff,sector*SD_BLOCK_SIZE,SD_BLOCK_SIZE,count);

      if (SD_state!=SD_RESPONSE_NO_ERROR)
          status = RES_PARERR;
      else
          status = RES_OK;
      break;

  case SPI_FLASH:
      break;

  default:
      status = RES_PARERR;
  }
  return status;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
  DRESULT status = RES_PARERR;
  switch (pdrv) {
  case DEV_SPI_SD: /* SD CARD */
      switch (cmd) {
      // Get R/W sector size (WORD)
      case GET_SECTOR_SIZE :
          *(WORD * )buff = SD_BLOCK_SIZE;
          break;
      // Get erase block size in unit of sector (DWORD)
      case GET_BLOCK_SIZE :
          *(DWORD * )buff = 1;
          break;

      case GET_SECTOR_COUNT:
  *(DWORD*)buff = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;
          break;
      case CTRL_SYNC :
          break;
      }
      status = RES_OK;
      break;

  case SPI_FLASH:
      break;

  default:
      status = RES_PARERR;
  }
  return status;
}
/*
get_fattime 函数用于获取当前时间戳，在 ff.c 文件中被调用。 FatFs 在文件创建、被修改时会记录时间，这里我们直接使用赋值方法
设定时间戳。为更好的记录时间，可以使用控制器 RTC 功能，具体要求返回值格式为：
        bit31:25 ——从 1980 至今是多少年，范围是 (0..127) ；
        bit24:21 ——月份，范围为 (1..12) ；
        bit20:16 ——该月份中的第几日，范围为(1..31) ；
        bit15:11——时，范围为 (0..23)；
        bit10:5 ——分，范围为 (0..59)；
        bit4:0 ——秒/ 2，范围为 (0..29)
*/
__weak DWORD get_fattime(void) {
	/* 返回当前时间戳*/
	return	  ((DWORD)(2025 - 1980) << 25)	/* Year 2025 */
			| ((DWORD)1 << 21)				/* Month 1 */
			| ((DWORD)1 << 16)				/* Mday 1 */
			| ((DWORD)0 << 11)				/* Hour 0 */
			| ((DWORD)0 << 5)				  /* Min 0 */
			| ((DWORD)0 >> 1);				/* Sec 0 */
}

