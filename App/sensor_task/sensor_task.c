/*
 * sensor_task.c
 *
 *  Created on: Jun 30, 2026
 *      Author: PV
 */

#include "sensor_task.h"
#include "i2c.h"
#include "stdbool.h"


Sens_List_Status_t 	sens_init_status;
Sens_Read_State_t	sens_read_state;

I2C_Handle_t 		onboard_sensor_i2c;
I2C_Handle_t		sensor_i2c;

LSM6DSOX_Handle_t 	lsm6dsox_dev;
BMP390_Handle_t		bmp390_dev;
H3LIS331DL_Handle_t	h3lis_dev;

TC1047_Handle_t 	Temp_HV_Channel;
TC1047_Handle_t 	Temp_LV_Channel;



/*-------------------------- INIT FUNTION --------------------------*/

void Sensor_ADC_Init(void){
	TC1047_Init(&Temp_LV_Channel, ADC_TEMP_HANDLE, ADC_TEMP_50V_CHANNEL,3300);
	TC1047_Init(&Temp_HV_Channel, ADC_TEMP_HANDLE, ADC_TEMP_300V_CHANNEL,3300);
}


void  Sensor_I2C_Init(void) {
	uint8_t init_ret = 0;

	I2C_Init(&onboard_sensor_i2c, ONBOARD_SENSOR_I2C_HANDLE);
	I2C_Init(&sensor_i2c, SENSOR_I2C_HANDLE);

	init_ret = LSM6DSOX_Init(&lsm6dsox_dev, &onboard_sensor_i2c);
	if(init_ret == I2C_Success) sens_init_status.lsm6d = SENS_INIT_OK;
	else sens_init_status.lsm6d = SENS_INIT_FAIL;

	init_ret = BMP390_init(&bmp390_dev, &sensor_i2c);
	if(init_ret == I2C_Success) sens_init_status.bmp390 = SENS_INIT_OK;
	else sens_init_status.bmp390 = SENS_INIT_FAIL;

	init_ret = H3LIS331DL_Init(&h3lis_dev, &onboard_sensor_i2c);
	if(init_ret == I2C_Success) sens_init_status.h3lis = SENS_INIT_OK;
	else sens_init_status.h3lis = SENS_INIT_FAIL;

}


/*-------------------------- TASK FUNTION --------------------------*/
void Sensor_ADC_task(void*){
	Temp_HV_Channel.temp_value = TC1047_GetTemperature(&Temp_HV_Channel);
	Temp_LV_Channel.temp_value = TC1047_GetTemperature(&Temp_LV_Channel);
}

void Sensor_I2C_task(void *argument) {

	switch (sens_read_state)
	{
	case SENS_STATE_READ_LSM6DSOX:
		if (sens_init_status.lsm6d == SENS_INIT_FAIL) {
			sens_read_state = SENS_STATE_READ_BMP390;
			return;
		}
		if (LSM6DSOX_Read_Data_IT(&lsm6dsox_dev) == I2C_Success) {
			sens_read_state = SENS_STATE_PROCESS_LSM6DSOX;
		}
		return;

	case SENS_STATE_PROCESS_LSM6DSOX:
		if (lsm6dsox_dev.dev_i2c->State != I2C_STATE_IDLE) {
			return;
		}
		LSM6DSOX_Process_Data_IT(&lsm6dsox_dev);
		sens_read_state = SENS_STATE_READ_BMP390;
		return;

	case SENS_STATE_READ_BMP390:
		if (sens_init_status.bmp390 == SENS_INIT_FAIL) {
			sens_read_state = SENS_STATE_READ_H3LIS331DL;
			return;
		}
		bmp390_temp_press_update_IT_Start(&bmp390_dev);
		sens_read_state = SENS_STATE_PROCESS_BMP390;
		return;

	case SENS_STATE_PROCESS_BMP390:
		if (bmp390_dev.dev_i2c->State != I2C_STATE_IDLE) {
			return;
		}

		bmp390_temp_press_update_IT_Complete(&bmp390_dev);
		sens_read_state = SENS_STATE_READ_H3LIS331DL;
		return;

	case SENS_STATE_READ_H3LIS331DL:
		if (sens_init_status.h3lis == SENS_INIT_FAIL) {
			sens_read_state = SENS_STATE_READ_LSM6DSOX;
			return;
		}

		if (H3LIS331DL_Get_Accel_IT_Start(&h3lis_dev) == I2C_Success) {
			sens_read_state = SENS_STATE_PROCESS_H3LIS331DL;
		}
		return;

	case SENS_STATE_PROCESS_H3LIS331DL:
		if (h3lis_dev.dev_i2c->State != I2C_STATE_IDLE) {
			return;
		}

		H3LIS331DL_Get_Accel_IT_Complete(&h3lis_dev);
		sens_read_state = SENS_STATE_READ_LSM6DSOX;
		return;

	default:
		return;
	}
}

//void Sensor_I2C_task(void*) {
//
//    if (sens_init_status.lsm6d != SENS_INIT_FAIL) {
//        LSM6DSOX_Read_Data(&hlsm6dsox_dev);
//    }
//    if (sens_init_status.bmp390 != SENS_INIT_FAIL) {
//        bmp390_temp_press_update(&hbmp390_dev);
//    }
//    if (sens_init_status.h3lis != SENS_INIT_FAIL) {
//        H3LIS331DL_Get_Accel(&hh3lis331dl_dev);
//    }
//
//}

/*-------------------------- ISR FUNTION --------------------------*/
void Sensor_I2C_EV_ISR(void){
	I2C_EV_IRQHandler(&sensor_i2c);
}


void Onboard_Sensor_I2C_EV_ISR(void){
	I2C_EV_IRQHandler(&onboard_sensor_i2c);
}

void Sensor_I2C_ER_ISR(void){
	I2C_EV_IRQHandler(&sensor_i2c);
}

void Onboard_Sensor_I2C_ER_ISR(void){
	I2C_EV_IRQHandler(&onboard_sensor_i2c);
}


