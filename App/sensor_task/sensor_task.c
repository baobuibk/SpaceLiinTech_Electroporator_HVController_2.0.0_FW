/*
 * sensor_task.c
 *
 *  Created on: Jun 30, 2026
 *      Author: PV
 */

#include "sensor_task.h"
#include "i2c.h"
#include "stdbool.h"

LSM6DSOX_Data_t LSM6DSOX_Data = {
    .Accel = {0, 0, 0},
    .Gyro =  {0, 0, 0}
};


H3LIS331DL_Data_t H3LIS331DL_Data = {
		.x = 0,
		.y = 0,
		.z = 0
};

BMP390_Value BMP390_Val = {
		.temperature = 0.0f,
		.pressure = 0.0f,
		.altitude = 0.0f
};

Sens_List_Status_t Sensor_list = {
		.lsm6d = 0,
		.h3lis = 0,
		.bmp390 = 0
};

void  Sensor_I2C_Init(void) {
	// LL_GPIO_SetOutputPin(SENSOR1_EN_GPIO_Port, SENSOR1_EN_Pin);
	// LL_GPIO_SetOutputPin(SENSOR2_EN_GPIO_Port, SENSOR2_EN_Pin);
	I2C_Status_t I2C_status = I2C_Error;
	bool bmp_ret = false;

	LL_mDelay(300);
	I2C_status = LSM6DSOX_Init();
	if (I2C_status == I2C_Success) Sensor_list.lsm6d = 1;
	else Sensor_list.lsm6d = 0;

	LL_mDelay(300);
	I2C_status = H3LIS331DL_Init();
	if (I2C_status == I2C_Success) Sensor_list.h3lis = 1;
	else Sensor_list.h3lis = 0;

	LL_mDelay(300);
	bmp_ret = BMP390_init();
	if (bmp_ret == true) Sensor_list.bmp390 = 1;
	else Sensor_list.bmp390 = 0;

}
void Sensor_I2C_task(void*){
//	LSM6DSOX_Read_Data(&LSM6DSOX_Data);
//	H3LIS331DL_Get_Accel(&H3LIS331DL_Data);
	bmp390_temp_press_update(&BMP390_Val);
}
