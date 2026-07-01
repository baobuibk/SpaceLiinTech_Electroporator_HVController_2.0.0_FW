/*
 * h3lis331dl.c
 *
 *  Created on: Apr 18, 2025
 *      Author: HTSANG
 */

#include "h3lis331dl.h"


H3LIS331DL_RawData_t H3LIS331DL_RawData = {
		.status = I2C_Success,
};

void H3LIS331DL_Write(uint8_t reg, uint8_t data)
{
	H3LIS331DL_RawData.status = I2C_Write(ONBOARD_SENSOR_I2C_HANDLE, H3LIS331DL_I2C_ADDR, reg, data);
}

void H3LIS331DL_Read(uint8_t reg, uint8_t *pData, uint8_t length)
{
	H3LIS331DL_RawData.status = I2C_ReadMulti(ONBOARD_SENSOR_I2C_HANDLE, H3LIS331DL_I2C_ADDR, reg, pData, length);
}

I2C_Status_t H3LIS331DL_Init(void)
{
	H3LIS331DL_Write(H3LIS331DL_CTRL_REG1, 0x3F);		// 0011 1111 (Normal mode, 1000Hz, enable XYZ)
	H3LIS331DL_Write(H3LIS331DL_CTRL_REG4, 0x80);		// 0011 0000 (FS = 400g)
	uint8_t ID = 0;
	H3LIS331DL_Read_ID(&ID);
	if (ID != H3LIS331DL_ID) H3LIS331DL_RawData.status = I2C_Error;
	return H3LIS331DL_RawData.status;
}

I2C_Status_t H3LIS331DL_Read_ID(uint8_t *ID)
{
	H3LIS331DL_Read(H3LIS331DL_ID_REG, ID, 1);  //ID Device: 0x32
	return H3LIS331DL_RawData.status;
}

void H3LIS331DL_Read_RawData(H3LIS331DL_RawData_t *RawData)
{
//	H3LIS331DL_Read(H3LIS331DL_OUT_X_L, RawData->accel_raw, 6);
	H3LIS331DL_Read(H3LIS331DL_OUT_X_L_MULTI, RawData->accel_raw, 6);


	int16_t x_raw = (int16_t)((RawData->accel_raw[1] << 8) | RawData->accel_raw[0]);
	int16_t y_raw = (int16_t)((RawData->accel_raw[3] << 8) | RawData->accel_raw[2]);
	int16_t z_raw = (int16_t)((RawData->accel_raw[5] << 8) | RawData->accel_raw[4]);

	    // Dịch phải 4 bit vì dữ liệu 12-bit được căn lề trái (Left-justified)
	RawData->acceleration[0] = x_raw >> 4;
	RawData->acceleration[1] = y_raw >> 4;
	RawData->acceleration[2] = z_raw >> 4;
}

I2C_Status_t H3LIS331DL_Get_Accel(H3LIS331DL_Data_t *Data)
{
	if (H3LIS331DL_RawData.status == I2C_Error) H3LIS331DL_Init();
	H3LIS331DL_Read_RawData(&H3LIS331DL_RawData);

	Data->x = (float)(H3LIS331DL_RawData.acceleration[0]) * H3LIS_SENSITIVITY_100G;
	Data->y = (float)(H3LIS331DL_RawData.acceleration[1]) * H3LIS_SENSITIVITY_100G;
	Data->z = (float)(H3LIS331DL_RawData.acceleration[2]) * H3LIS_SENSITIVITY_100G;
	return H3LIS331DL_RawData.status;
}
