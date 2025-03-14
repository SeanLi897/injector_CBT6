#include "i2c_soft.h"

/**
 * @brief  I2C delay
 * @param  None
 * @retval None
 */
void I2C_Delay(void) {
  uint8_t i = 0;
  for (i = 0; i < 10; i++)
    ;
}

/**
 * @brief  I2C start signal
 * @param  None
 * @retval None
 */
void I2C_Start(void) {
  SDA_OUT();
  SDA_H();
  SCL_H();
  I2C_Delay();
  SDA_L();
  I2C_Delay();
  SCL_L();
  I2C_Delay();
}

/**
 * @brief  I2C stop signal
 * @param  None
 * @retval None
 */
void I2C_Stop(void) {
  SDA_OUT();
  SCL_L();
  SDA_L();
  I2C_Delay();
  SCL_H();
  SDA_H();
  I2C_Delay();
}

/**
 * @brief  I2C send ACK signal
 * @param  None
 * @retval None
 */
void I2C_Ack(void) {
  SCL_L();
  SDA_OUT();
  SDA_L();
  I2C_Delay();
  SCL_H();
  I2C_Delay();
  SCL_L();
  I2C_Delay();
}

/**
 * @brief  I2C send NACK signal
 * @param  None
 * @retval None
 */
void I2C_NAck(void) {
  SCL_L();
  SDA_OUT();
  SDA_H();
  I2C_Delay();
  SCL_H();
  I2C_Delay();
  SCL_L();
  I2C_Delay();
}

/**
 * @brief  I2C wait ACK signal
 * @param  None
 * @retval 1: ACK 0: NACK
 */
uint8_t I2C_WaitAck(void) {
  uint8_t ucErrTime = 0;
  SDA_IN();
  SDA_H();
  I2C_Delay();
  SCL_H();
  I2C_Delay();
  while (SDA_READ()) {
    ucErrTime++;
    if (ucErrTime > 250) {
      I2C_Stop();
      return 1;
    }
  }
  SCL_L();
  I2C_Delay();
  return 0;
}

/**
 * @brief  I2C send byte
 * @param  sendbyte: byte to send
 * @retval None
 */
void I2C_SendByte(uint8_t SendByte) {
  uint8_t i = 0;
  SDA_OUT();
  SCL_L();
  for (i = 0; i < 8; i++) {
    if (SendByte & 0x80)
      SDA_H();
    else
      SDA_L();
    SendByte <<= 1;
    I2C_Delay();
    SCL_H();
    I2C_Delay();
    SCL_L();
    I2C_Delay();
  }
}

/**
 * @brief  I2C read byte
 * @param  None
 * @retval read byte
 */
uint8_t I2C_ReadByte(void) {
  uint8_t i = 0, ReceiveByte = 0;
  SDA_IN();
  for (i = 0; i < 8; i++) {
    SCL_L();
    I2C_Delay();
    SCL_H();
    ReceiveByte <<= 1;
    if (SDA_READ())
      ReceiveByte++;
    I2C_Delay();
  }
  SCL_L();
  return ReceiveByte;
}

/**
 * @brief  I2C write data
 * @param  SlaveAddress: slave address
 * @param  Data: data buffer
 * @param  len: data length
 * @retval 0: success 1: fail
 */
uint8_t I2C_WriteData(uint8_t SlaveAddress, uint8_t *Data, uint8_t len) {
  uint8_t i = 0;
  I2C_Start();
  I2C_SendByte(SlaveAddress);
  if (I2C_WaitAck()) {
    I2C_Stop();
    return 1;
  }
  for (i = 0; i < len; i++) {
    I2C_SendByte(Data[i]);
    if (I2C_WaitAck()) {
      I2C_Stop();
      return 1;
    }
  }
  I2C_Stop();
  return 0;
}

/**
 * @brief  I2C read data
 * @param  SlaveAddress: slave address
 * @param  Data: data buffer
 * @param  len: data length
 * @retval 0: success 1: fail
 */
uint8_t I2C_ReadData(uint8_t SlaveAddress, uint8_t *Data, uint8_t len) {
  uint8_t i = 0;
  I2C_Start();
  I2C_SendByte(SlaveAddress + 1);
  if (I2C_WaitAck()) {
    I2C_Stop();
    return 1;
  }
  for (i = 0; i < len; i++) {
    Data[i] = I2C_ReadByte();
    if (i == (len - 1))
      I2C_NAck();
    else
      I2C_Ack();
  }
  I2C_Stop();
  return 0;
}
