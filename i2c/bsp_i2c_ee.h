#ifndef __I2C_EE_H
#define	__I2C_EE_H


#include "stm32f10x.h"


/**************************I2C参数定义，I2C1或I2C2********************************/
#define             macI2Cx                                I2C1
#define             macI2C_APBxClock_FUN                   RCC_APB1PeriphClockCmd
#define             macI2C_CLK                             RCC_APB1Periph_I2C1
#define             macI2C_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macI2C_GPIO_CLK                        RCC_APB2Periph_GPIOB     
#define             macI2C_SCL_PORT                        GPIOB   
#define             macI2C_SCL_PIN                         GPIO_Pin_6
#define             macI2C_SDA_PORT                        GPIOB 
#define             macI2C_SDA_PIN                         GPIO_Pin_7
#define				WR	0x0
#define 			RD  0x1

/* 
 * AT24C02 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */

/* EEPROM Addresses defines */
#define macEEPROM_Block0_ADDRESS 0x50   /* E2 = 0 */
//#define macEEPROM_Block1_ADDRESS 0xA2 /* E2 = 0 */
//#define macEEPROM_Block2_ADDRESS 0xA4 /* E2 = 0 */
//#define macEEPROM_Block3_ADDRESS 0xA6 /* E2 = 0 */


void I2C_EE_Init(void);
void I2C_EE_BufferWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite);
void I2C_EE_ByteWrite(u8* pBuffer, u8 WriteAddr);
void I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite);
void I2C_EE_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead);
void I2C_EE_WaitEepromStandbyState(void);
uint8_t I2C_ReadByte(u8* pBuffer,u8 ReadAddr);
uint8_t I2C_ReadByte_s(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead);
uint8_t I2C_WriteByte_s(uint8_t salve_addr,uint8_t* data_buffer,u8 NumByteToWrite);
#endif /* __I2C_EE_H */
