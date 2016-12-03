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



void I2C_BNO_Init(void);
uint8_t I2C_ReadByte(u8* pBuffer,u8 ReadAddr);
uint8_t I2C_ReadByte_s(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead);
uint8_t I2C_WriteByte_s(uint8_t salve_addr,uint8_t* data_buffer,u8 NumByteToWrite);

#endif /* __I2C_EE_H */
