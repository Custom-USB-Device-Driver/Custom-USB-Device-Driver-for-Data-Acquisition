
#include "mpu6050.h"

void MPU6050_Init(I2C_HandleTypeDef *hi2c) {
													  uint8_t data = 0x00;
													/* Wake up MPU6050 */
												HAL_I2C_Mem_Write(hi2c,MPU6050_ADDR,MPU6050_PWR1,1,&data,1,HAL_MAX_DELAY);

}

void MPU6050_Read_Accel(I2C_HandleTypeDef *hi2c,int16_t *Ax,int16_t *Ay,int16_t *Az)
{

		uint8_t buffer[6];
		HAL_I2C_Mem_Read(hi2c,MPU6050_ADDR,ACCEL_XOUT_H,1,buffer,6,HAL_MAX_DELAY);
		*Ax = (int16_t)(buffer[0]<<8 | buffer[1]);
		*Ay = (int16_t)(buffer[2]<<8 | buffer[3]);
		*Az = (int16_t)(buffer[4]<<8 | buffer[5]);

}
