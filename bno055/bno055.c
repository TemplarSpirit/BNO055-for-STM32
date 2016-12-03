#include "bno055.h"
#include <stdio.h>
#include "bsp_i2c_ee.h"
#include "delay.h"

#define BNO055_CHIP_ID		0xA0

bno055_struct bno055;

short offset[11]={0xFFF3,0xFFF2,0x000C,0x00C4,0x00D1,0x0097,0xFFFF,0x0,0x0,0x03E8,0x0239};

u8 	bno055_read_page_id(void)
{
	u8 temp;
	I2C_ReadByte(&temp,BNO055_PAGE_ID_ADDR);
	return temp;
}

/* */
void 	bno055_write_page_id(u8* page_id)
{
	I2C_WriteByte_s(BNO055_PAGE_ID_ADDR,page_id,1);
}

/* befroe read offset profile 
	need switch to CONFING mode */
uint8_t  bno055_get_acc_offset(short* acc_offset)
{
	u8 temp[6],i;
	/* memeroy clear */
	for(i=0;i<6;i++)
		temp[i]=0;
	if(I2C_ReadByte_s(temp,ACCEL_OFFSET_X_LSB_ADDR,6))
		return 1;
	acc_offset[0]=(short)((temp[1] << 8) | temp[0]);	//x
	acc_offset[1]=(short)((temp[3] << 8) | temp[2]);	//y
	acc_offset[2]=(short)((temp[5] << 8) | temp[4]);	//z
	return 0;
}

uint8_t  bno055_get_gry_offset(short* gyr_offset)
{
	u8 temp[6],i;
	/* memeroy clear */
	for(i=0;i<6;i++)
		temp[i]=0;
	if(I2C_ReadByte_s(temp,GYRO_OFFSET_X_LSB_ADDR,6))
		return 1;
	gyr_offset[0]=(short)((temp[1] << 8) | temp[0]);	//x
	gyr_offset[1]=(short)((temp[3] << 8) | temp[2]);	//y
	gyr_offset[2]=(short)((temp[5] << 8) | temp[4]);	//z
	return 0;
}


uint8_t  bno055_get_mag_offset(short* mag_offset)
{
	u8 temp[6],i;
	/* memeroy clear */
	for(i=0;i<6;i++)
		temp[i]=0;
	if(I2C_ReadByte_s(temp,MAG_OFFSET_X_LSB_ADDR,6))
		return 1;
	mag_offset[0]=(short)((temp[1] << 8) | temp[0]);	//x
	mag_offset[1]=(short)((temp[3] << 8) | temp[2]);	//y
	mag_offset[2]=(short)((temp[5] << 8) | temp[4]);	//z
	return 0;
}

uint8_t  bno055_get_accradius_offset(short* acc_magradius)
{
	u8 temp[4],i;
	/* memeroy clear */
	for(i=0;i<6;i++)
		temp[i]=0;
	if(I2C_ReadByte_s(temp,ACCEL_RADIUS_LSB_ADDR,4))
		return 1;
	acc_magradius[0]=(short)((temp[1] << 8) | temp[0]);	// acc radius	
	acc_magradius[1]=(short)((temp[3] << 8) | temp[2]);	// mag radius
	
	return 0;
}

	

u8 BNO055_Init(void)
{
	u8 temp,i;
	short * p = bno055.acc_offset;
	bno055.mode=BNO055_NOF_MODE;
	for(i=0;i<11;i++){
		*p=offset[i];
		p++;
	}
	/* Check device ID */
	I2C_ReadByte(&temp,BNO055_CHIP_ID_ADDR);
	if(temp != BNO055_CHIP_ID)
		return 1;
	
	/* Check Self test status */
	I2C_ReadByte(&temp,BNO055_SELFTEST_RESULT_ADDR);
	if((temp & 0x0F)!=0x0F)
		return 2;
	//bno055_set_sensor_offset(&bno055);
	/* Set sensor mode */
	if(I2C_WriteByte_s(BNO055_OPR_MODE_ADDR,&bno055.mode,1))
		GPIOB->BRR = GPIO_Pin_5;
	delay_ms(50); //from CONFIGMODE to any mode time 
	
	/* Check Calibration status 
		and wait untill calibrate done */
	I2C_ReadByte(&temp,BNO055_CALIB_STAT_ADDR);
	while((temp & 0xFF) != 0xFF){
		GPIOB->BSRR = GPIO_Pin_5;
		delay_ms(200);
		//I2C_ReadByte(&temp,BNO055_SYS_STAT_ADDR);
		GPIOB->BRR = GPIO_Pin_5;
		delay_ms(200);
		I2C_ReadByte(&temp,BNO055_CALIB_STAT_ADDR);
	}
	/* Get three sensor offset for reuser 
		*Need switch CONGIF mode */
	bno055.mode=BNO055_CONFIG_MODE;
	temp=0;
	if(I2C_WriteByte_s(BNO055_OPR_MODE_ADDR,&bno055.mode,1))
		return 1;
	delay_ms(50);
	temp += bno055_get_acc_offset(bno055.acc_offset);
	temp += bno055_get_gry_offset(bno055.gry_offset);
	temp += bno055_get_mag_offset(bno055.mag_offset);
	temp += bno055_get_accradius_offset(bno055.radius);
	if(temp)
		return 1;
	/* switch to fusion  mode */
	bno055.mode=BNO055_NOF_MODE;
	if(I2C_WriteByte_s(BNO055_OPR_MODE_ADDR,&bno055.mode,1))
		return 1;
	return 0;
}

/*
	when run in NOF mode data ouput rates is 100HZ */
void get_euler_angles(signed short * euler_data)
{
	u8 temp[6],i;
	
	for(i=0;i<6;i++)
		temp[i]=0;
	
	I2C_ReadByte_s(temp,BNO055_EULER_H_LSB_ADDR,6);
	euler_data[0]=(short)((temp[1] << 8) | temp[0]);	//heading
	euler_data[1]=(short)((temp[3] << 8) | temp[2]);	//roll
	euler_data[2]=(short)((temp[5] << 8) | temp[4]);	//pitch
	
}

uint8_t bno055_set_sensor_offset(bno055_struct * bno)
{
	uint8_t temp=0;
	
	bno->mode=BNO055_CONFIG_MODE;
	/* switch to CONFIG mode */
	if(I2C_WriteByte_s(BNO055_OPR_MODE_ADDR,&bno->mode,1))
		return 1;
	delay_ms(50);
	/* Set offset */
	temp += I2C_WriteByte_s(ACCEL_OFFSET_X_LSB_ADDR,((uint8_t *)(&(*bno).acc_offset)),18);
	temp += I2C_WriteByte_s(ACCEL_OFFSET_X_LSB_ADDR,((uint8_t *)(&(*bno).radius)),4);
	if(temp)
		return 1;
	/* switch to NOF mode */
	bno->mode=BNO055_NOF_MODE;
	if(I2C_WriteByte_s(BNO055_OPR_MODE_ADDR,&bno->mode,1))
		return 1;
	
	return 0;
}


