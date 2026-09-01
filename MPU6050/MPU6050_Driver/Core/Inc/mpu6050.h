/*
 * mpu6050.h
 *
 *  Created on: 22-Jun-2026
 *      Author: gauri96
 */
#ifndef MPU6050_H
#define MPU6050_H

#include "main.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

#define MPU6050_ADDR        (0x68 << 1)
#define MPU6050_PWR1        0x6B
#define ACCEL_XOUT_H        0x3B
void MPU6050_Init(I2C_HandleTypeDef *hi2c);
void MPU6050_Read_Accel(I2C_HandleTypeDef *hi2c,int16_t *Ax,int16_t *Ay,int16_t *Az);
#endif

