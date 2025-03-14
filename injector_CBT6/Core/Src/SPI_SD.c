
#include "SPI_SD.h"

//#define ENTER_SD_BLOCK_SIZE      (512)
//#define ENTER_SD_SECTOR_SIZE     (ENTER_SD_BLOCK_SIZE)

#define SD_CS_HIGH()         HAL_GPIO_WritePin(TF_CS_GPIO_Port, TF_CS_Pin, GPIO_PIN_SET);		//CS置位（拉高）
#define SD_CS_LOW()          HAL_GPIO_WritePin(TF_CS_GPIO_Port, TF_CS_Pin, GPIO_PIN_RESET);	//CS复位（拉低）

//记录卡的类型
uint8_t  SD_Type = SD_TYPE_NOT_SD;	//存储卡的类型
SD_CardInfo SDCardInfo;	//用于存储卡的信息

// SPI速度控制
void SD_SPI_SpeedLow(void) {
  hspi1.Instance->CR1 &= ~SPI_BAUDRATEPRESCALER_256;
  hspi1.Instance->CR1 |= SPI_BAUDRATEPRESCALER_256; // 64MHz/256 = 250kHz
  HAL_SPI_Init(&hspi1);
}

void SD_SPI_SpeedHigh(void) {
  hspi1.Instance->CR1 &= ~SPI_BAUDRATEPRESCALER_256;
  hspi1.Instance->CR1 |= SPI_BAUDRATEPRESCALER_4; // 64MHz/4 = 25MHz
  HAL_SPI_Init(&hspi1);
}

SD_Error SD_Init(void)
{
  uint32_t i = 0;

  /*!< Initialize SD_SPI */
//  GPIO_Configuration();

  /*!< SD chip select high */
  SD_CS_HIGH();

  /*!< Send dummy byte 0xFF, 10 times with CS high */
  /*!< Rise CS and MOSI for 80 clocks cycles */
  for (i = 0; i <= 9; i++)
  {
    /*!< Send dummy byte 0xFF */
    SD_WriteByte(SD_DUMMY_BYTE);
  }

	//获取卡的类型，最多尝试10次
	i=0;
	do
	{
		/*------------Put SD in SPI mode--------------*/
		/*!< SD initialized and set to SPI mode properly */
		SD_GoIdleState();

		/*Get card type*/
		SD_GetCardType();

	}while(SD_Type == SD_TYPE_NOT_SD && i++ >10);

	//不支持的卡
	if(SD_Type == SD_TYPE_NOT_SD)
		return SD_RESPONSE_FAILURE;

	return SD_GetCardInfo(&SDCardInfo);

}

/**
 * @brief  Detect if SD card is correctly plugged in the memory slot.
 * @param  None
 * @retval Return if SD is detected or not
 */
uint8_t SD_Detect(void)
{
  __IO uint8_t status = SD_PRESENT;

  /*!< Check GPIO to detect SD */
//  if (GPIO_ReadInputData(SD_DETECT_GPIO_PORT) & SD_DETECT_PIN)
//  {
//    status = SD_NOT_PRESENT;
//  }
  return status;
}

/**
  * @brief  Returns information about specific card.
  * @param  cardinfo: pointer to a SD_CardInfo structure that contains all SD
  *         card information.
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo)
{
  SD_Error status = SD_RESPONSE_FAILURE;

  status = SD_GetCSDRegister(&(cardinfo->SD_csd));
  status = SD_GetCIDRegister(&(cardinfo->SD_cid));

	if ((SD_Type == SD_TYPE_V1) || (SD_Type == SD_TYPE_V2))
	{

  cardinfo->CardCapacity = (cardinfo->SD_csd.DeviceSize + 1) ;
  cardinfo->CardCapacity *= (1 << (cardinfo->SD_csd.DeviceSizeMul + 2));
  cardinfo->CardBlockSize = 1 << (cardinfo->SD_csd.RdBlockLen);
  cardinfo->CardCapacity *= cardinfo->CardBlockSize;

	}
	else if (SD_Type == SD_TYPE_V2HC)
	{
	  cardinfo->CardCapacity = (uint64_t)(cardinfo->SD_csd.DeviceSize + 1) * 512 * 1024;
    cardinfo->CardBlockSize = 512;
	}

  /*!< Returns the reponse */
  return status;
}

/**
  * @brief  Reads a block of data from the SD.
  * @param  pBuffer: pointer to the buffer that receives the data read from the
  *                  SD.
  * @param  ReadAddr: SD's internal address to read from.
  * @param  BlockSize: the SD card Data block size.
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Error SD_ReadBlock(uint8_t* pBuffer, uint64_t ReadAddr, uint16_t BlockSize)
{
  uint32_t i = 0;
  SD_Error rvalue = SD_RESPONSE_FAILURE;

	//SDHC卡块大小固定为512，且读命令中的地址单位是sector
	if (SD_Type == SD_TYPE_V2HC)
  {
    BlockSize = 512;
    ReadAddr /= 512;
  }

  /*!< SD chip select low */
  SD_CS_LOW();

  /*!< Send CMD17 (SD_CMD_READ_SINGLE_BLOCK) to read one block */
  SD_SendCmd(SD_CMD_READ_SINGLE_BLOCK, ReadAddr, 0xFF);

  /*!< Check if the SD acknowledged the read block command: R1 response (0x00: no errors) */
  if (!SD_GetResponse(SD_RESPONSE_NO_ERROR))
  {
    /*!< Now look for the data token to signify the start of the data */
    if (!SD_GetResponse(SD_START_DATA_SINGLE_BLOCK_READ))
    {
      /*!< Read the SD block data : read NumByteToRead data */
      for (i = 0; i < BlockSize; i++)
      {
        /*!< Save the received data */
        *pBuffer = SD_ReadByte();

        /*!< Point to the next location where the byte read will be saved */
        pBuffer++;
      }
      /*!< Get CRC bytes (not really needed by us, but required by SD) */
      SD_ReadByte();
      SD_ReadByte();
      /*!< Set response value to success */
      rvalue = SD_RESPONSE_NO_ERROR;
    }
  }
  /*!< SD chip select high */
  SD_CS_HIGH();

  /*!< Send dummy byte: 8 Clock pulses of delay */
  SD_WriteByte(SD_DUMMY_BYTE);

  /*!< Returns the reponse */
  return rvalue;
}

/**
  * @brief  Reads multiple block of data from the SD.
  * @param  pBuffer: pointer to the buffer that receives the data read from the
  *                  SD.
  * @param  ReadAddr: SD's internal address to read from.
  * @param  BlockSize: the SD card Data block size.
  * @param  NumberOfBlocks: number of blocks to be read.
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Error SD_ReadMultiBlocks(uint8_t* pBuffer, uint64_t ReadAddr, uint16_t BlockSize, uint32_t NumberOfBlocks)
{
  uint32_t i = 0, Offset = 0;
  SD_Error rvalue = SD_RESPONSE_FAILURE;

	//SDHC卡块大小固定为512，且读命令中的地址单位是sector
	if (SD_Type == SD_TYPE_V2HC)
  {
    BlockSize = 512;
    ReadAddr /= 512;
  }

  /*!< SD chip select low */
  SD_CS_LOW();
  /*!< Data transfer */
  while (NumberOfBlocks--)
  {
    /*!< Send CMD17 (SD_CMD_READ_SINGLE_BLOCK) to read one block */
    SD_SendCmd (SD_CMD_READ_SINGLE_BLOCK, ReadAddr + Offset, 0xFF);
    /*!< Check if the SD acknowledged the read block command: R1 response (0x00: no errors) */
    if (SD_GetResponse(SD_RESPONSE_NO_ERROR))
    {
      return  SD_RESPONSE_FAILURE;
    }
    /*!< Now look for the data token to signify the start of the data */
    if (!SD_GetResponse(SD_START_DATA_SINGLE_BLOCK_READ))
    {
      /*!< Read the SD block data : read NumByteToRead data */
      for (i = 0; i < BlockSize; i++)
      {
        /*!< Read the pointed data */
        *pBuffer = SD_ReadByte();
        /*!< Point to the next location where the byte read will be saved */
        pBuffer++;
      }
      /*!< Set next read address*/
      Offset += BlockSize;
      /*!< get CRC bytes (not really needed by us, but required by SD) */
      SD_ReadByte();
      SD_ReadByte();
      /*!< Set response value to success */
      rvalue = SD_RESPONSE_NO_ERROR;
    }
    else
    {
      /*!< Set response value to failure */
      rvalue = SD_RESPONSE_FAILURE;
    }

    /* Ìí¼Ó Send dummy byte 防止读操作失败*/
    SD_WriteByte(SD_DUMMY_BYTE);
  }
  /*!< SD chip select high */
  SD_CS_HIGH();
  /*!< Send dummy byte: 8 Clock pulses of delay */
  SD_WriteByte(SD_DUMMY_BYTE);
  /*!< Returns the reponse */
  return rvalue;
}

/**
  * @brief  Writes a block on the SD
  * @param  pBuffer: pointer to the buffer containing the data to be written on
  *                  the SD.
  * @param  WriteAddr: address to write on.
  * @param  BlockSize: the SD card Data block size.
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Error SD_WriteBlock(uint8_t* pBuffer, uint64_t WriteAddr, uint16_t BlockSize)
{
  uint32_t i = 0;
  SD_Error rvalue = SD_RESPONSE_FAILURE;

	//SDHC卡块大小固定为512，且读命令中的地址单位是sector
	if (SD_Type == SD_TYPE_V2HC)
  {
    BlockSize = 512;
    WriteAddr /= 512;
  }

  /*!< SD chip select low */
  SD_CS_LOW();

  /*!< Send CMD24 (SD_CMD_WRITE_SINGLE_BLOCK) to write multiple block */
  SD_SendCmd(SD_CMD_WRITE_SINGLE_BLOCK, WriteAddr, 0xFF);

  /*!< Check if the SD acknowledged the write block command: R1 response (0x00: no errors) */
  if (!SD_GetResponse(SD_RESPONSE_NO_ERROR))
  {
    /*!< Send a dummy byte */
    SD_WriteByte(SD_DUMMY_BYTE);

    /*!< Send the data token to signify the start of the data */
    SD_WriteByte(0xFE);

    /*!< Write the block data to SD : write count data by block */
    for (i = 0; i < BlockSize; i++)
    {
      /*!< Send the pointed byte */
      SD_WriteByte(*pBuffer);
      /*!< Point to the next location where the byte read will be saved */
      pBuffer++;
    }
    /*!< Put CRC bytes (not really needed by us, but required by SD) */
    SD_ReadByte();
    SD_ReadByte();

    /*!< Read data response */
    if (SD_GetDataResponse() == SD_DATA_OK)
    {
      rvalue = SD_RESPONSE_NO_ERROR;
    }
  }
  /*!< SD chip select high */
  SD_CS_HIGH();
  /*!< Send dummy byte: 8 Clock pulses of delay */
  SD_WriteByte(SD_DUMMY_BYTE);

  /*!< Returns the reponse */
  return rvalue;
}

/**
  * @brief  Writes many blocks on the SD
  * @param  pBuffer: pointer to the buffer containing the data to be written on
  *                  the SD.
  * @param  WriteAddr: address to write on.
  * @param  BlockSize: the SD card Data block size.
  * @param  NumberOfBlocks: number of blocks to be written.
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Error SD_WriteMultiBlocks(uint8_t* pBuffer, uint64_t WriteAddr, uint16_t BlockSize, uint32_t NumberOfBlocks)
{
  uint32_t i = 0, Offset = 0;
  SD_Error rvalue = SD_RESPONSE_FAILURE;

	//SDHC卡块大小固定为512，且读命令中的地址单位是sector
	if (SD_Type == SD_TYPE_V2HC)
  {
    BlockSize = 512;
    WriteAddr /= 512;
  }

  /*!< SD chip select low */
  SD_CS_LOW();
  /*!< Data transfer */
  while (NumberOfBlocks--)
  {
    /*!< Send CMD24 (SD_CMD_WRITE_SINGLE_BLOCK) to write blocks */
    SD_SendCmd(SD_CMD_WRITE_SINGLE_BLOCK, WriteAddr + Offset, 0xFF);
    /*!< Check if the SD acknowledged the write block command: R1 response (0x00: no errors) */
    if (SD_GetResponse(SD_RESPONSE_NO_ERROR))
    {
      return SD_RESPONSE_FAILURE;
    }
    /*!< Send dummy byte */
    SD_WriteByte(SD_DUMMY_BYTE);
    /*!< Send the data token to signify the start of the data */
    SD_WriteByte(SD_START_DATA_SINGLE_BLOCK_WRITE);
    /*!< Write the block data to SD : write count data by block */
    for (i = 0; i < BlockSize; i++)
    {
      /*!< Send the pointed byte */
      SD_WriteByte(*pBuffer);
      /*!< Point to the next location where the byte read will be saved */
      pBuffer++;
    }
    /*!< Set next write address */
    Offset += BlockSize;
    /*!< Put CRC bytes (not really needed by us, but required by SD) */
    SD_ReadByte();
    SD_ReadByte();
    /*!< Read data response */
    if (SD_GetDataResponse() == SD_DATA_OK)
    {
      /*!< Set response value to success */
      rvalue = SD_RESPONSE_NO_ERROR;
    }
    else
    {
      /*!< Set response value to failure */
      rvalue = SD_RESPONSE_FAILURE;
    }
  }
  /*!< SD chip select high */
  SD_CS_HIGH();
  /*!< Send dummy byte: 8 Clock pulses of delay */
  SD_WriteByte(SD_DUMMY_BYTE);
  /*!< Returns the reponse */
  return rvalue;
}




// 在diskio.c中添加调试输出
void SD_Debug_Response(uint8_t cmd, uint8_t response) {
    printf("CMD%d 响应: 0x%02X\r\n", cmd, response);
}

/**
  * @brief  Read the CSD card register.
  *         Reading the contents of the CSD register in SPI mode is a simple
  *         read-block transaction.
  * @param  SD_csd: pointer on an SCD register structure
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Error SD_GetCSDRegister(SD_CSD* SD_csd)
{
  uint32_t i = 0;
  SD_Error rvalue = SD_RESPONSE_FAILURE;
  uint8_t CSD_Tab[16];

  /*!< SD chip select low */
  SD_CS_LOW();
  /*!< Send CMD9 (CSD register) or CMD10(CSD register) */
  SD_SendCmd(SD_CMD_SEND_CSD, 0, 0xFF);
  /*!< Wait for response in the R1 format (0x00 is no errors) */
  if (!SD_GetResponse(SD_RESPONSE_NO_ERROR))
  {
    if (!SD_GetResponse(SD_START_DATA_SINGLE_BLOCK_READ))
    {
      for (i = 0; i < 16; i++)
      {
        /*!< Store CSD register value on CSD_Tab */
        CSD_Tab[i] = SD_ReadByte();
      }
    }
    /*!< Get CRC bytes (not really needed by us, but required by SD) */
    SD_WriteByte(SD_DUMMY_BYTE);
    SD_WriteByte(SD_DUMMY_BYTE);
    /*!< Set response value to success */
    rvalue = SD_RESPONSE_NO_ERROR;
  }
  /*!< SD chip select high */
  SD_CS_HIGH();
  /*!< Send dummy byte: 8 Clock pulses of delay */
  SD_WriteByte(SD_DUMMY_BYTE);

  /*!< Byte 0 */
  SD_csd->CSDStruct = (CSD_Tab[0] & 0xC0) >> 6;
  SD_csd->SysSpecVersion = (CSD_Tab[0] & 0x3C) >> 2;
  SD_csd->Reserved1 = CSD_Tab[0] & 0x03;

  /*!< Byte 1 */
  SD_csd->TAAC = CSD_Tab[1];

  /*!< Byte 2 */
  SD_csd->NSAC = CSD_Tab[2];

  /*!< Byte 3 */
  SD_csd->MaxBusClkFrec = CSD_Tab[3];

  /*!< Byte 4 */
  SD_csd->CardComdClasses = CSD_Tab[4] << 4;

  /*!< Byte 5 */
  SD_csd->CardComdClasses |= (CSD_Tab[5] & 0xF0) >> 4;
  SD_csd->RdBlockLen = CSD_Tab[5] & 0x0F;

  /*!< Byte 6 */
  SD_csd->PartBlockRead = (CSD_Tab[6] & 0x80) >> 7;
  SD_csd->WrBlockMisalign = (CSD_Tab[6] & 0x40) >> 6;
  SD_csd->RdBlockMisalign = (CSD_Tab[6] & 0x20) >> 5;
  SD_csd->DSRImpl = (CSD_Tab[6] & 0x10) >> 4;
  SD_csd->Reserved2 = 0; /*!< Reserved */

  SD_csd->DeviceSize = (CSD_Tab[6] & 0x03) << 10;

	//V1与SDSC卡的信息
  if ((SD_Type == SD_TYPE_V1) || (SD_Type == SD_TYPE_V2))
	{
		/*!< Byte 7 */
		SD_csd->DeviceSize |= (CSD_Tab[7]) << 2;

		/*!< Byte 8 */
		SD_csd->DeviceSize |= (CSD_Tab[8] & 0xC0) >> 6;

		SD_csd->MaxRdCurrentVDDMin = (CSD_Tab[8] & 0x38) >> 3;
		SD_csd->MaxRdCurrentVDDMax = (CSD_Tab[8] & 0x07);

		/*!< Byte 9 */
		SD_csd->MaxWrCurrentVDDMin = (CSD_Tab[9] & 0xE0) >> 5;
		SD_csd->MaxWrCurrentVDDMax = (CSD_Tab[9] & 0x1C) >> 2;
		SD_csd->DeviceSizeMul = (CSD_Tab[9] & 0x03) << 1;
		/*!< Byte 10 */
		SD_csd->DeviceSizeMul |= (CSD_Tab[10] & 0x80) >> 7;
  }
	//SDHC卡的信息
	else if (SD_Type == SD_TYPE_V2HC)
	{
		SD_csd->DeviceSize = (CSD_Tab[7] & 0x3F) << 16;

		SD_csd->DeviceSize |= (CSD_Tab[8] << 8);

		SD_csd->DeviceSize |= (CSD_Tab[9]);
	}


  SD_csd->EraseGrSize = (CSD_Tab[10] & 0x40) >> 6;
  SD_csd->EraseGrMul = (CSD_Tab[10] & 0x3F) << 1;

  /*!< Byte 11 */
  SD_csd->EraseGrMul |= (CSD_Tab[11] & 0x80) >> 7;
  SD_csd->WrProtectGrSize = (CSD_Tab[11] & 0x7F);

  /*!< Byte 12 */
  SD_csd->WrProtectGrEnable = (CSD_Tab[12] & 0x80) >> 7;
  SD_csd->ManDeflECC = (CSD_Tab[12] & 0x60) >> 5;
  SD_csd->WrSpeedFact = (CSD_Tab[12] & 0x1C) >> 2;
  SD_csd->MaxWrBlockLen = (CSD_Tab[12] & 0x03) << 2;

  /*!< Byte 13 */
  SD_csd->MaxWrBlockLen |= (CSD_Tab[13] & 0xC0) >> 6;
  SD_csd->WriteBlockPaPartial = (CSD_Tab[13] & 0x20) >> 5;
  SD_csd->Reserved3 = 0;
  SD_csd->ContentProtectAppli = (CSD_Tab[13] & 0x01);

  /*!< Byte 14 */
  SD_csd->FileFormatGrouop = (CSD_Tab[14] & 0x80) >> 7;
  SD_csd->CopyFlag = (CSD_Tab[14] & 0x40) >> 6;
  SD_csd->PermWrProtect = (CSD_Tab[14] & 0x20) >> 5;
  SD_csd->TempWrProtect = (CSD_Tab[14] & 0x10) >> 4;
  SD_csd->FileFormat = (CSD_Tab[14] & 0x0C) >> 2;
  SD_csd->ECC = (CSD_Tab[14] & 0x03);

  /*!< Byte 15 */
  SD_csd->CSD_CRC = (CSD_Tab[15] & 0xFE) >> 1;
  SD_csd->Reserved4 = 1;

  /*!< Return the reponse */
  return rvalue;
}

/**
  * @brief  Read the CID card register.
  *         Reading the contents of the CID register in SPI mode is a simple
  *         read-block transaction.
  * @param  SD_cid: pointer on an CID register structure
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Error SD_GetCIDRegister(SD_CID* SD_cid)
{
  uint32_t i = 0;
  SD_Error rvalue = SD_RESPONSE_FAILURE;
  uint8_t CID_Tab[16];

  /*!< SD chip select low */
  SD_CS_LOW();

  /*!< Send CMD10 (CID register) */
  SD_SendCmd(SD_CMD_SEND_CID, 0, 0xFF);

  /*!< Wait for response in the R1 format (0x00 is no errors) */
  if (!SD_GetResponse(SD_RESPONSE_NO_ERROR))
  {
    if (!SD_GetResponse(SD_START_DATA_SINGLE_BLOCK_READ))
    {
      /*!< Store CID register value on CID_Tab */
      for (i = 0; i < 16; i++)
      {
        CID_Tab[i] = SD_ReadByte();
      }
    }
    /*!< Get CRC bytes (not really needed by us, but required by SD) */
    SD_WriteByte(SD_DUMMY_BYTE);
    SD_WriteByte(SD_DUMMY_BYTE);
    /*!< Set response value to success */
    rvalue = SD_RESPONSE_NO_ERROR;
  }
  /*!< SD chip select high */
  SD_CS_HIGH();
  /*!< Send dummy byte: 8 Clock pulses of delay */
  SD_WriteByte(SD_DUMMY_BYTE);

  /*!< Byte 0 */
  SD_cid->ManufacturerID = CID_Tab[0];

  /*!< Byte 1 */
  SD_cid->OEM_AppliID = CID_Tab[1] << 8;

  /*!< Byte 2 */
  SD_cid->OEM_AppliID |= CID_Tab[2];

  /*!< Byte 3 */
  SD_cid->ProdName1 = CID_Tab[3] << 24;

  /*!< Byte 4 */
  SD_cid->ProdName1 |= CID_Tab[4] << 16;

  /*!< Byte 5 */
  SD_cid->ProdName1 |= CID_Tab[5] << 8;

  /*!< Byte 6 */
  SD_cid->ProdName1 |= CID_Tab[6];

  /*!< Byte 7 */
  SD_cid->ProdName2 = CID_Tab[7];

  /*!< Byte 8 */
  SD_cid->ProdRev = CID_Tab[8];

  /*!< Byte 9 */
  SD_cid->ProdSN = CID_Tab[9] << 24;

  /*!< Byte 10 */
  SD_cid->ProdSN |= CID_Tab[10] << 16;

  /*!< Byte 11 */
  SD_cid->ProdSN |= CID_Tab[11] << 8;

  /*!< Byte 12 */
  SD_cid->ProdSN |= CID_Tab[12];

  /*!< Byte 13 */
  SD_cid->Reserved1 |= (CID_Tab[13] & 0xF0) >> 4;
  SD_cid->ManufactDate = (CID_Tab[13] & 0x0F) << 8;

  /*!< Byte 14 */
  SD_cid->ManufactDate |= CID_Tab[14];

  /*!< Byte 15 */
  SD_cid->CID_CRC = (CID_Tab[15] & 0xFE) >> 1;
  SD_cid->Reserved2 = 1;

  /*!< Return the reponse */
  return rvalue;
}

/**
  * @brief  Send 5 bytes command to the SD card.
  * @param  Cmd: The user expected command to send to SD card.
  * @param  Arg: The command argument.
  * @param  Crc: The CRC.
  * @retval None
  */
void SD_SendCmd(uint8_t Cmd, uint32_t Arg, uint8_t Crc)
{
  uint32_t i = 0x00;

  uint8_t Frame[6];

  Frame[0] = (Cmd | 0x40); /*!< Construct byte 1 */

  Frame[1] = (uint8_t)(Arg >> 24); /*!< Construct byte 2 */

  Frame[2] = (uint8_t)(Arg >> 16); /*!< Construct byte 3 */

  Frame[3] = (uint8_t)(Arg >> 8); /*!< Construct byte 4 */

  Frame[4] = (uint8_t)(Arg); /*!< Construct byte 5 */

  Frame[5] = (Crc); /*!< Construct CRC: byte 6 */

  for (i = 0; i < 6; i++)
  {
    SD_WriteByte(Frame[i]); /*!< Send the Cmd bytes */
  }
}

/**
  * @brief  Get SD card data response.
  * @param  None
  * @retval The SD status: Read data response xxx0<status>1
  *         - status 010: Data accecpted
  *         - status 101: Data rejected due to a crc error
  *         - status 110: Data rejected due to a Write error.
  *         - status 111: Data rejected due to other error.
  */
uint8_t SD_GetDataResponse(void)
{
  uint32_t i = 0;
  uint8_t response, rvalue;

  while (i <= 64)
  {
    /*!< Read resonse */
    response = SD_ReadByte();
    /*!< Mask unused bits */
    response &= 0x1F;
    switch (response)
    {
      case SD_DATA_OK:
      {
        rvalue = SD_DATA_OK;
        break;
      }
      case SD_DATA_CRC_ERROR:
        return SD_DATA_CRC_ERROR;
      case SD_DATA_WRITE_ERROR:
        return SD_DATA_WRITE_ERROR;
      default:
      {
        rvalue = SD_DATA_OTHER_ERROR;
        break;
      }
    }
    /*!< Exit loop in case of data ok */
    if (rvalue == SD_DATA_OK)
      break;
    /*!< Increment loop counter */
    i++;
  }

  /*!< Wait null data */
  while (SD_ReadByte() == 0);

  /*!< Return response */
  return response;
}

/**
  * @brief  Returns the SD response.
  * @param  None
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Error SD_GetResponse(uint8_t Response)
{
  uint32_t Count = 0xFFF;

  /*!< Check if response is got or a timeout is happen */
  while ((SD_ReadByte() != Response) && Count)
  {
    Count--;
  }
  if (Count == 0)
  {
    /*!< After time out */
    return SD_RESPONSE_FAILURE;
  }
  else
  {
    /*!< Right response got */
    return SD_RESPONSE_NO_ERROR;
  }
}

/**
  * @brief  Returns the SD status.
  * @param  None
  * @retval The SD status.
  */
uint16_t SD_GetStatus(void)
{
  uint16_t Status = 0;

  /*!< SD chip select low */
  SD_CS_LOW();

  /*!< Send CMD13 (SD_SEND_STATUS) to get SD status */
  SD_SendCmd(SD_CMD_SEND_STATUS, 0, 0xFF);

  Status = SD_ReadByte();
  Status |= (uint16_t)(SD_ReadByte() << 8);

  /*!< SD chip select high */
  SD_CS_HIGH();

  /*!< Send dummy byte 0xFF */
  SD_WriteByte(SD_DUMMY_BYTE);

  return Status;
}

/**
  * @brief  获取SD卡的版本类型，并区分SDSC和SDHC
  * @param  无
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Error SD_GetCardType(void)
{
  uint32_t i = 0;
	uint32_t Count = 0xFFF;

  uint8_t R7R3_Resp[4];
	uint8_t R1_Resp;

	SD_CS_HIGH();

	/*!< Send Dummy byte 0xFF */
	SD_WriteByte(SD_DUMMY_BYTE);

	/*!< SD chip select low */
	SD_CS_LOW();


  /*!< Send CMD8 */
  SD_SendCmd(SD_CMD_SEND_IF_COND, 0x1AA, 0x87);

  /*!< Check if response is got or a timeout is happen */
  while (( (R1_Resp = SD_ReadByte()) == 0xFF) && Count)
  {
    Count--;
  }
  if (Count == 0)
  {
    /*!< After time out */
    return SD_RESPONSE_FAILURE;
  }

	//响应 = 0x05 非V2.0的卡
	if(R1_Resp == (SD_IN_IDLE_STATE|SD_ILLEGAL_COMMAND))
	{
		  /*----------Activates the card initialization process-----------*/
		do
		{
			/*!< SD chip select high */
			SD_CS_HIGH();

			/*!< Send Dummy byte 0xFF */
			SD_WriteByte(SD_DUMMY_BYTE);

			/*!< SD chip select low */
			SD_CS_LOW();

			/*!< 发送CMD1完成V1版本卡的初始化 */
			SD_SendCmd(SD_CMD_SEND_OP_COND, 0, 0xFF);
			/*!< Wait for no error Response (R1 Format) equal to 0x00 */
		}
		while (SD_GetResponse(SD_RESPONSE_NO_ERROR));
		//V1版本的卡完成初始化

		SD_Type = SD_TYPE_V1;

		//不处理MMC卡

		//初始化正常

	}
	//响应 0x01 V2.0的卡
  else if (R1_Resp == SD_IN_IDLE_STATE)
  {
      /*!< 读取CMD8的R7响应 */
      for (i = 0; i < 4; i++)
      {
        R7R3_Resp[i] = SD_ReadByte();
      }

			/*!< SD chip select high */
			SD_CS_HIGH();

			/*!< Send Dummy byte 0xFF */
			SD_WriteByte(SD_DUMMY_BYTE);

			/*!< SD chip select low */
			SD_CS_LOW();

			//判断该卡是否支持2.7-3.6V电压
			if(R7R3_Resp[2]==0x01 && R7R3_Resp[3]==0xAA)
			{
					//支持电压范围，可以操作
					Count = 200;
					//发卡初始化指令CMD55+ACMD41
				do
    		{
					//CMD55，以强调下面的是ACMD命令
    			SD_SendCmd(SD_CMD_APP_CMD, 0, 0xFF);
					if (!SD_GetResponse(SD_RESPONSE_NO_ERROR)) // SD_IN_IDLE_STATE
						return SD_RESPONSE_FAILURE; //超时返回

					//ACMD41命令带HCS检查位
    			SD_SendCmd(SD_ACMD_SD_SEND_OP_COND, 0x40000000, 0xFF);

					if(Count-- == 0)
						return SD_RESPONSE_FAILURE; //重试次数超时
         }while(SD_GetResponse(SD_RESPONSE_NO_ERROR));

				 //³õÊ¼»¯Ö¸ÁîÍê³É£¬¶ÁÈ¡OCRÐÅÏ¢£¬CMD58

				 //-----------鉴别SDSC SDHC卡类型开始-----------

				 Count = 200;
				 do
					{
						/*!< SD chip select high */
						SD_CS_HIGH();

						/*!< Send Dummy byte 0xFF */
						SD_WriteByte(SD_DUMMY_BYTE);

						/*!< SD chip select low */
						SD_CS_LOW();

						/*!< 发送CMD58 读取OCR寄存器 */
						SD_SendCmd(SD_CMD_READ_OCR, 0, 0xFF);
					}
					while ( SD_GetResponse(SD_RESPONSE_NO_ERROR) || Count-- == 0);

					if(Count == 0)
						return SD_RESPONSE_FAILURE; //重试次数超时

					//响应正常，读取R3响应

					  /*!< 读取CMD58的R3响应 */
						for (i = 0; i < 4; i++)
						{
							R7R3_Resp[i] = SD_ReadByte();
						}

						//检查接收到OCR中bit30(CCS)
						//CCS = 0:SDSC			 CCS = 1:SDHC
            if(R7R3_Resp[0]&0x40)    //检查CCS标志
            {
                SD_Type = SD_TYPE_V2HC;
            }
            else
            {
                SD_Type = SD_TYPE_V2;
            }
            //-----------鉴别SDSC SDHC版本卡的流程结束-----------
			}
  }

	/*!< SD chip select high */
  SD_CS_HIGH();
  /*!< Send dummy byte: 8 Clock pulses of delay */
  SD_WriteByte(SD_DUMMY_BYTE);

	//初始化正常返回
	return SD_RESPONSE_NO_ERROR;
}

/**
  * @brief  Put SD in Idle state.
  * @param  None
  * @retval The SD Response:
  *         - SD_RESPONSE_FAILURE: Sequence failed
  *         - SD_RESPONSE_NO_ERROR: Sequence succeed
  */
SD_Error SD_GoIdleState(void)
{
  /*!< SD chip select low */
  SD_CS_LOW();

  /*!< Send CMD0 (SD_CMD_GO_IDLE_STATE) to put SD in SPI mode */
  SD_SendCmd(SD_CMD_GO_IDLE_STATE, 0, 0x95);

  /*!< Wait for In Idle State Response (R1 Format) equal to 0x01 */
  if (SD_GetResponse(SD_IN_IDLE_STATE))
  {
    /*!< No Idle State Response: return response failue */
    return SD_RESPONSE_FAILURE;
  }

	SD_CS_HIGH();

	/*!< Send Dummy byte 0xFF */
	SD_WriteByte(SD_DUMMY_BYTE);


	//正常返回
	return SD_RESPONSE_NO_ERROR ;
}

/**
  * @brief  Write a byte on the SD.
  * @param  Data: byte to send.
  * @retval None
  */
//uint8_t SD_WriteByte(uint8_t Data)
//{
//  /*!< Wait until the transmit buffer is empty */
//  while(SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_TXE) == RESET)
//  {
//  }
//
//  /*!< Send the byte */
//  SPI_I2S_SendData(SD_SPI, Data);
//
//  /*!< Wait to receive a byte*/
//  while(SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_RXNE) == RESET)
//  {
//  }
//
//  /*!< Return the byte read from the SPI bus */
//  return SPI_I2S_ReceiveData(SD_SPI);
//}

// SPI单字节传输
uint8_t SD_WriteByte(uint8_t data) {
  uint8_t rx;
  HAL_SPI_TransmitReceive(&hspi1, &data, &rx, 1, 100);
  return rx;
}

/**
  * @brief  Read a byte from the SD.
  * @param  None
  * @retval The received byte.
  */
uint8_t SD_ReadByte(void)
{
  uint8_t Data = 0;

  Data = SD_WriteByte(SD_DUMMY_BYTE);

  return Data;
}

/**
  * @brief  读取文件数据，通过循环读取一小部分数据实现
  * @param  FIL* fp:文件对象指针
  * @param  void* buff:存放读出数据的首地址
  * @param  UINT btr:读取数据的字节数
  * @param  UINT* br:指示成功读取的字节个数
  * @retval 操作结果
  */

FRESULT f_read_co(FIL* fp, void* buff, UINT btr, UINT* br)
{
	FRESULT rc = FR_OK; 	 // 函数返回值
	UINT ui_index = 0; 		 // 当前要读的位置
	UINT ui_read_back = 0; // 从f_read()返回的当前读了多少个字节
	UINT ui_read_once = 0; // 本地调用f_read(),要读取多少字节

	if (NULL != br) {
		*br = 0;// 如果传入的bw参数不为空，则将其值设置为0
	}

	for (ui_index = 0; ui_index < btr;) {
		// 计算本次调用f_read()要写入多少字节，最多为SD_BLOCKSIZE
		ui_read_once = ((btr - ui_index) >= SD_BLOCK_SIZE) ? SD_BLOCK_SIZE : (btr - ui_index);
		ui_read_back = 0;// 每次循环前将ui_read_back重置为0
		rc = f_read(fp, (BYTE*)buff + ui_index, ui_read_once, &ui_read_back);// 调用f_read()读取数据


		ui_index += ui_read_back;// 更新已读取的字节数

		if (NULL != br) {
			*br += ui_read_back;// 如果传入的bw参数不为空，更新bw的值
		}

		if (ui_read_once != ui_read_back) {
			break; // 如果本次写入未达到预期字节数，跳出循环，表示写入完成
		}
	}

	return rc;// 返回函数执行状态
}

/**
  * @brief  写入文件数据，通过循环写入一小部分数据实现
  * @param  FIL* fp:文件对象指针
  * @param  void* buff:存放写入数据的首地址
  * @param  UINT btr:写入数据的字节数
  * @param  UINT* br:指示成功写入的字节个数
  * @retval 操作结果
  */

FRESULT f_write_co(FIL* fp, const void* buff, UINT btw, UINT* bw)
{
	FRESULT rc = FR_OK; // 函数返回值
	UINT ui_index = 0; // 当前要写的内容的位置索引
	UINT ui_write_back = 0; // 由f_write返回的写了多少字节
	UINT ui_write_once = 0; // 一次写多少个字节，必须<= 512字节,否则FatFs写失败（只有前512字节正确，后面都是0）

	if (NULL != bw) {
		*bw = 0;//如果传入的bw参数不为空，则将其值设置为0
	}

	for (ui_index = 0; ui_index < btw;) {
		//计算本次调用f_write()要写入多少字节，最多为SD_BLOCKSIZE
		ui_write_once = ((btw - ui_index) >= SD_BLOCK_SIZE) ? SD_BLOCK_SIZE : (btw - ui_index);
		ui_write_back = 0;//每次循环前将ui_write_back重置为0
		rc = f_write(fp, (BYTE*)buff + ui_index, ui_write_once, &ui_write_back);//调用f_write()写入数据


		ui_index += ui_write_back;// 更新已写入的字节数

		if (NULL != bw) {
			*bw += ui_write_back;//如果传入的bw参数不为空，更新bw的值
		}

		if (ui_write_once != ui_write_back) {
			break; //如果本次写入未达到预期字节数，跳出循环，表示写入完成
		}
	}

	return rc;// 返回函数执行状态
}
/**
  * @}
  */


/**
  * @}
  */

