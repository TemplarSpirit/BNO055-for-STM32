/**
  ******************************************************************************
  * @file    bsp_i2c_ee.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   i2c EEPROM(AT24C02)Ӧ�ú���bsp
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "bsp_i2c_ee.h"
		

/* STM32 I2C ����ģʽ */
#define I2C_Speed              200000  //*

/* �����ַֻҪ��STM32��ҵ�I2C������ַ��һ������ */
#define I2Cx_OWN_ADDRESS7      0X0A   

/* AT24C01/02ÿҳ��8���ֽ� */
#define I2C_PageSize           8

/* AT24C04/08A/16Aÿҳ��16���ֽ� */
//#define I2C_PageSize           16	


uint16_t EEPROM_ADDRESS;

uint8_t temp_offset[18];

/**
  * @brief  I2C1 I/O����
  * @param  ��
  * @retval ��
  */
static void I2C_GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 

	/* ʹ���� I2C1 �йص�ʱ�� */
	macI2C_APBxClock_FUN ( macI2C_CLK, ENABLE );
	macI2C_GPIO_APBxClock_FUN ( macI2C_GPIO_CLK, ENABLE );
	
    
  /* PB6-I2C1_SCL��PB7-I2C1_SDA*/
  GPIO_InitStructure.GPIO_Pin = macI2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       // ��©���
  GPIO_Init(macI2C_SCL_PORT, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = macI2C_SDA_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       // ��©���
  GPIO_Init(macI2C_SDA_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	       // ��©���
  GPIO_Init(macI2C_SCL_PORT, &GPIO_InitStructure);
	GPIOB->BSRR = GPIO_Pin_5;
}


/**
  * @brief  I2C ����ģʽ����
  * @param  ��
  * @retval ��
  */
static void I2C_Mode_Configu(void)
{
  I2C_InitTypeDef  I2C_InitStructure; 

  /* I2C ���� */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	
	/* �ߵ�ƽ�����ȶ����͵�ƽ���ݱ仯 SCL ʱ���ߵ�ռ�ձ� */
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	
  I2C_InitStructure.I2C_OwnAddress1 =I2Cx_OWN_ADDRESS7; 
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
	
	/* I2C��Ѱַģʽ */
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	
	/* ͨ������ */
  I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;
  
	/* I2C1 ��ʼ�� */
  I2C_Init(macI2Cx, &I2C_InitStructure);
  
	/* ʹ�� I2C1 */
  I2C_Cmd(macI2Cx, ENABLE);   
}


/**
  * @brief  I2C ����(EEPROM)��ʼ��
  * @param  ��
  * @retval ��
  */
void I2C_EE_Init(void)
{

  I2C_GPIO_Config(); 
 
  I2C_Mode_Configu();

/* ����ͷ�ļ�i2c_ee.h�еĶ�����ѡ��EEPROMҪд��ĵ�ַ */
#ifdef macEEPROM_Block0_ADDRESS
  /* ѡ�� EEPROM Block0 ��д�� */
  EEPROM_ADDRESS = 0x50;
#endif

#ifdef macEEPROM_Block1_ADDRESS  
	/* ѡ�� EEPROM Block1 ��д�� */
  EEPROM_ADDRESS = macEEPROM_Block1_ADDRESS;
#endif

#ifdef macEEPROM_Block2_ADDRESS  
	/* ѡ�� EEPROM Block2 ��д�� */
  EEPROM_ADDRESS = macEEPROM_Block2_ADDRESS;
#endif

#ifdef macEEPROM_Block3_ADDRESS  
	/* ѡ�� EEPROM Block3 ��д�� */
  EEPROM_ADDRESS = macEEPROM_Block3_ADDRESS;
#endif
}

uint8_t I2C_WriteByte_s(uint8_t salve_addr,uint8_t* data_buffer,u8 NumByteToWrite)
{
	volatile uint32_t Timeout = 0xFFFF;
	volatile uint32_t temp = 0;
	/*  Busy check */
	while((I2C1->SR2)&0x0002); 

	I2C1->CR1 |= 0x0100;
	while((I2C1->SR1&0x0001) != 0x0001){
		if(Timeout-- == 0)
			return 1;
	}
	Timeout = 0xFFFF;
	/* Send slave address */
    /* Reset the address bit0 for write*/
	I2C1->DR = EEPROM_ADDRESS | WR;
	/* Test on EV6 and clear it */
	while((I2C1->SR1 & 0x0002)!= 0x0002){
	  if(Timeout-- == 0)
		return 1;
	}
	/* Clear EV6 by setting again the PE bit */
	temp = I2C1->SR2;
	/* Send the BNO055 register address to write to */
	I2C1->DR = salve_addr;
	while((I2C1->SR1&0x0004)!=0x0004);
	
	/* While there is data to be written */
	while(NumByteToWrite){
		/* Send the current byte */
		temp_offset[NumByteToWrite-1] = *data_buffer;
		I2C1->DR = *data_buffer;
		 /* Point to the next byte to be written */
		data_buffer++;
		/* Poll on BTF to receive data because in polling mode we can not guarantee the
        EV8 software sequence is managed before the current byte transfer completes */
		while((I2C1->SR1&0x0004)!=0x0004);
		NumByteToWrite--;
	}
	/* EV8_2: Wait until BTF is set before programming the STOP 
	while((I2C1->SR1 & 0x0004)!= 0x0004);*/
	/* Send STOP condition */
	I2C1->CR1 |= 0x0200;
	/* Make sure that the STOP bit is cleared by Hardware */
	while((I2C1->CR1 & 0x0200)==0x0200);
	return 0;
}

uint8_t I2C_ReadByte(u8* pBuffer,u8 ReadAddr)
{

	volatile uint32_t Timeout = 0xFFFF;
	volatile uint32_t temp = 0;
	/*  Busy check */
	while((I2C1->SR2)&0x0002);    

	/* Send START condition */
	I2C1->CR1 |= 0x0100;
	 /* Wait until SB flag is set: EV5  */
	while ((I2C1->SR1&0x0001) != 0x0001){
		if (Timeout-- == 0)
			return 1;
	}
	/* Send EEPROM address for write */
	I2C1->DR = EEPROM_ADDRESS | WR;
	Timeout = 0xFFFF;
	/* Test on EV6 and clear it */
	while((I2C1->SR1 & 0x0002)!= 0x0002){
	  if(Timeout-- == 0)
		return 1;
	}
	/* Clear EV6 by setting again the PE bit */
	temp = I2C1->SR2;
	/* Send the EEPROM's internal address to write to */
	I2C1->DR =  ReadAddr;
	/* EV8_2: Wait until BTF is set before programming the STOP */
	while ((I2C1->SR1 & 0x00004) != 0x000004);
	Timeout = 0xFFFF;
	/* Send START condition */
	I2C1->CR1 |= 0x0100;
	/* Test on EV5 and clear it */
	while ((I2C1->SR1&0x0001) != 0x0001){
		if (Timeout-- == 0)
			return 1;
	}
	 /* Send slave address */
    /* Reset the address bit0 for read */
	I2C1->DR = EEPROM_ADDRESS | RD;
	/* Wait until ADDR is set: EV6_3, then program ACK = 0, clear ADDR
		and program the STOP just after ADDR is cleared. The EV6_3 
		software sequence must complete before the current byte end of transfer.*/
	/* Wait until ADDR is set */
	Timeout = 0xFFFF;
	while ((I2C1->SR1&0x0002) != 0x0002){
		if (Timeout-- == 0)
			return 1;
	}
	/* Clear ACK bit */
	I2C1->CR1 &= 0xFBFF;
	/* Clear ADDR flag */
    temp = I2C1->SR2;
	/* Program the STOP */
    I2C1->CR1 |= 0x0200;
	/* Wait until a data is received in DR register (RXNE = 1) EV7 */
    while ((I2C1->SR1 & 0x0040) != 0x0040);
	/* Read the data */
	*pBuffer = I2C1->DR;
	/* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
	while ((I2C1->CR1&0x200) == 0x200);
	/* Enable Acknowledgement to be ready for another reception */
	I2C1->CR1 |= 0x0400;

	return 0;
}

uint8_t I2C_ReadByte_s(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead)
{
	volatile uint32_t Timeout = 0xFFFF;
	volatile uint32_t temp = 0;
	
	if(NumByteToRead ==1)
		return 1;
	//*((u8 *)0x4001080c) |=0x80; 
	while((I2C1->SR2)&0x0002); // Added by Najoua 27/08/2008    

	/* Send START condition */
	I2C1->CR1 |= 0x0100;
	/* Test on EV5 and clear it */
	while ((I2C1->SR1&0x0001) != 0x0001){
		if (Timeout-- == 0)
			return 1;
	}
	/* Send EEPROM address for write */
	I2C1->DR = EEPROM_ADDRESS | WR;
	Timeout = 0xFFFF;
	/* Test on EV6 and clear it */
	while((I2C1->SR1 & 0x0002)!= 0x0002){
	  if(Timeout-- == 0)
		return 1;
	}
	/* Clear EV6 by setting again the PE bit */
	temp = I2C1->SR2;
	/* Send the EEPROM's internal address to write to */
	I2C1->DR =  ReadAddr;
	/* EV8_2: Wait until BTF is set before programming the STOP */
	while ((I2C1->SR1 & 0x00004) != 0x000004);
	Timeout = 0xFFFF;
	/******************************************************/
	if(NumByteToRead ==2){
		/* Send START condition */
		I2C1->CR1 |= 0x0100;
		/* Test on EV5 and clear it */
		while ((I2C1->SR1&0x0001) != 0x0001){
			if (Timeout-- == 0)
				return 1;
		}
		/* Send slave address */
		/* Reset the address bit0 for read */
		I2C1->DR = EEPROM_ADDRESS | RD;
		/* Wait until ADDR is set: EV6_3, then program ACK = 0, clear ADDR
			and program the STOP just after ADDR is cleared. The EV6_3 
			software sequence must complete before the current byte end of transfer.*/
		/* Wait until ADDR is set */
		Timeout = 0xFFFF;
		while ((I2C1->SR1&0x0002) != 0x0002){
			if (Timeout-- == 0)
				return 1;
		}
		/* Clear ADDR by reading SR2 register  */
		temp = I2C1->SR2;
		/* Clear ACK */
		I2C1->CR1 &= 0xFBFF;	
		/* Wait until BTF is set */
		while ((I2C1->SR1 & 0x00004) != 0x000004);
		/* Program the STOP */
		I2C1->CR1 |= 0x0200;
		/* Read first data */
		*pBuffer = I2C1->DR;
		/**/
		pBuffer++;
		/* Read second data */
		*pBuffer = I2C1->DR;
		/* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
		while ((I2C1->CR1&0x200) == 0x200);
		/* Enable Acknowledgement to be ready for another reception */
		I2C1->CR1  |= 0x0400;
		
		return 0;
	}
	else{
		/* Send START condition */
		I2C1->CR1 |= 0x0100;
		/* Test on EV5 and clear it */
		while ((I2C1->SR1&0x0001) != 0x0001){
			if (Timeout-- == 0)
				return 1;
		}
		/* Send slave address */
		/* Reset the address bit0 for read */
		I2C1->DR = EEPROM_ADDRESS | RD;
		/* Wait until ADDR is set: EV6_3, then program ACK = 0, clear ADDR
			and program the STOP just after ADDR is cleared. The EV6_3 
			software sequence must complete before the current byte end of transfer.*/
		/* Wait until ADDR is set */
		Timeout = 0xFFFF;
		while ((I2C1->SR1&0x0002) != 0x0002){
			if (Timeout-- == 0)
				return 1;
		}
		/* Clear ADDR by reading SR2 register  */
		temp = I2C1->SR2;
		while(NumByteToRead){
			/* Receive bytes from first byte until byte N-3 */
			if(NumByteToRead !=3){
				/* Poll on BTF to receive data because in polling mode we can not guarantee the
				EV7 software sequence is managed before the current byte transfer completes */
				while ((I2C1->SR1 & 0x00004) != 0x000004);
				/* Read data */
				*pBuffer = I2C1->DR;
				/* */
				pBuffer++;
				/* Decrement the read bytes counter */
				NumByteToRead--;
			}
			/* it remains to read three data: data N-2, data N-1, Data N */
			if (NumByteToRead == 3){
				/* Wait until BTF is set: Data N-2 in DR and data N -1 in shift register */
				while ((I2C1->SR1 & 0x00004) != 0x000004);
				/* Clear ACK */
				I2C1->CR1 &= 0xFBFF;
				/* Read Data N-2 */
				*pBuffer = I2C1->DR;
				/* Increment */
				pBuffer++;
				/* Program the STOP */
				I2C1->CR1 |= 0x0200;
				/* Read DataN-1 */
				*pBuffer = I2C1->DR;
				/* Increment */
				pBuffer++;
				/* Wait until RXNE is set (DR contains the last data) */
				while ((I2C1->SR1 & 0x0040) != 0x0040);
				/* Read DataN */
				*pBuffer = I2C1->DR;
				/* Reset the number of bytes to be read by master */
				NumByteToRead = 0;
			}
		}
		/* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
		while ((I2C1->CR1&0x200) == 0x200);
		/* Enable Acknowledgement to be ready for another reception */
		I2C1->CR1 |= 0x0400;
		return 0;
	}
	
}












