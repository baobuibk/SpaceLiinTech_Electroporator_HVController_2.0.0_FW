/*
 * sensor_task.c
 *
 *  Created on: Jun 30, 2026
 *      Author: PV
 */

#include "sensor_task.h"
#include "i2c.h"
#include "stdbool.h"


TC1047_Handle_t Temp_HV_Channel;
TC1047_Handle_t Temp_LV_Channel;

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

/*-------------------------- INIT FUNTION --------------------------*/

void Sensor_ADC_Init(void){
	TC1047_Init(&Temp_LV_Channel, ADC_TEMP_HANDLE, ADC_TEMP_50V_CHANNEL,3300);
	TC1047_Init(&Temp_HV_Channel, ADC_TEMP_HANDLE, ADC_TEMP_300V_CHANNEL,3300);
}


void  Sensor_I2C_Init(void) {

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


/*-------------------------- TASK FUNTION --------------------------*/
void Sensor_I2C_task(void*){
	LSM6DSOX_Read_Data(&LSM6DSOX_Data);
	H3LIS331DL_Get_Accel(&H3LIS331DL_Data);
	bmp390_temp_press_update(&BMP390_Val);
}

void Sensor_ADC_task(void*){
	Temp_HV_Channel.temp_value = TC1047_GetTemperature(&Temp_HV_Channel);
	Temp_LV_Channel.temp_value = TC1047_GetTemperature(&Temp_LV_Channel);
}


