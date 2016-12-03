
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsP_i2c_ee.h"
#include "bno055.h"
#include "delay.h"
#include <string.h>

extern bno055_struct bno055;
void I2C_Test(void);


int main(void)
{ 
  u8 data=0;
	u8 i=BNO055_CONFIG_MODE,j=0x20;
	double pitch=0,roll=0,yaw=0;
  /* 串口1初始化 */
	delay_init();
	USARTx_Config();
	
	/* I2C 外设初(AT24C02)始化 */
	I2C_BNO_Init();
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
