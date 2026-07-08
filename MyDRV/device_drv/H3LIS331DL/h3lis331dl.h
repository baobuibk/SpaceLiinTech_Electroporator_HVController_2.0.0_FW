/*
 * h3lis331dl.h
 *
 *  Created on: Apr 18, 2025
 *      Author: HTSANG
 */

#ifndef SENSOR_I2C_H3LIS331DL_H3LIS331DL_H_
#define SENSOR_I2C_H3LIS331DL_H3LIS331DL_H_

#include "main.h"
#include "i2c.h"
#include "stdbool.h"
#include "stdint.h"
#include "board.h"


#define H3LIS331DL_ID						0x32

#define H3LIS331DL_ID_REG                   0x0F
/** I2C Device Address 8 bit format  if SA0=0 -> 0x18 if SA0=1 -> 0x19 **/
// #define H3LIS331DL_I2C_ADDR					0x18
#define H3LIS331DL_I2C_ADDR					0x19
#define H3LIS331DL_CTRL_REG1				0x20
#define H3LIS331DL_CTRL_REG2				0x21
#define H3LIS331DL_CTRL_REG4				0x23
#define H3LIS331DL_OUT_X_L					0x28
#define H3LIS331DL_OUT_X_L_MULTI			0xA8

#define H3LIS_SENSITIVITY_100G              49.0f
#define H3LIS_SENSITIVITY_200G              98.0f
#define H3LIS_SENSITIVITY_400G              195.0f

// Khai báo Enum cấu hình mức Full Scale cho IC
typedef enum {
	H3LIS331DL_FS_100G = 0x00, // FS bits: 00
	H3LIS331DL_FS_200G = 0x10, // FS bits: 01
	H3LIS331DL_FS_400G = 0x30  // FS bits: 11
} H3LIS331DL_FS_t;

typedef struct H3LIS331DL_RawData
{
	uint8_t accel_raw[6];
	int16_t acceleration[3];
	float sensitivity;         // Hệ số chuyển đổi mg/digit hiện hành
	I2C_Status_t status;
}H3LIS331DL_RawData_t;

typedef struct H3LIS331DL_Data
{
	float x;
	float y;
	float z;
} H3LIS331DL_Data_t;

I2C_Status_t H3LIS331DL_Init(void);
I2C_Status_t H3LIS331DL_Read_ID(uint8_t *ID);
I2C_Status_t H3LIS331DL_Get_Accel(H3LIS331DL_Data_t *Data);

I2C_Status_t H3LIS331DL_Set_FS(H3LIS331DL_FS_t fs);
H3LIS331DL_FS_t H3LIS331DL_Get_FS(void);

#endif /* SENSOR_I2C_H3LIS331DL_H3LIS331DL_H_ */