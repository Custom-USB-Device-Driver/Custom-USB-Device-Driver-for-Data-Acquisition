/*
 * mpu6050.h
 *
 *  Created on: 22-Jun-2026
 *      Author: gauri96
 */

#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "stm32f4xx_hal.h"



typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
}MPU6050_t;


HAL_StatusTypeDef MPU6050_IsReady(I2C_HandleTypeDef *hi2c);

HAL_StatusTypeDef MPU6050_ReadWhoAmI(I2C_HandleTypeDef *hi2c,
                                     uint8_t *id);

HAL_StatusTypeDef MPU6050_Init(I2C_HandleTypeDef *hi2c);

HAL_StatusTypeDef MPU6050_ReadAccel(I2C_HandleTypeDef *hi2c,
                                    MPU6050_t *mpu);

#endif
