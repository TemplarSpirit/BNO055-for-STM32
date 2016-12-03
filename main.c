/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   I2C EEPROM(AT24C02)测试，测试信息通过USART1打印在电脑的超级终端
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsP_i2c_ee.h"
#include "bno055.h"
#include "delay.h"
#include <string.h>

#define  EEP_Firstpage      0x00
u8 I2c_Buf_Write[256];
u8 I2c_Buf_Read[256];
extern bno055_struct bno055;
void I2C_Test(void);

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{ 
  u8 data=0;
	u8 i=BNO055_CONFIG_MODE,j=0x20;
	double pitch=0,roll=0,yaw=0;
  /* 串口1初始化 */
//	USART1_Config();
	delay_init();
	USARTx_Config();
	
	//printf("\r\n 这是一个I2C外设(AT24C02)读写测试例程 \r\n");
	//LED_GPIO_Config();

	/* I2C 外设初(AT24C02)始化 */
	I2C_EE_Init();
	delay_ms(1000);
	/* at least 650ms from reset to config mode */
	while(BNO055_Init()){
		printf("devier init error\r\n");
		I2C_WriteByte_s(BNO055_OPR_MODE_ADDR,&i,1);
		I2C_WriteByte_s(BNO055_SYS_TRIGGER_ADDR,&j,1);
		delay_ms(1000);
	}
	delay_ms(1000);
  while (1){      
		get_euler_angles(bno055.eular);
		for(i=0;i<3;i++)
			printf("euler [%d]:%d\r\n",i,bno055.eular[i]);
		yaw=(double)bno055.eular[0]/16.0;
		roll=(double)bno055.eular[1]/16.0;
		pitch=(double)bno055.eular[2]/16.0;
		printf("roll %0.2f,pitch %0.2f,yaw %0.2f\r\n",roll,pitch,yaw);
		delay_ms(200);
  }
}


/*********************************************END OF FILE**********************/
