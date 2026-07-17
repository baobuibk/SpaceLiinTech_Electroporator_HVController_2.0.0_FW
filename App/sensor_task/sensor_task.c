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

static 	uint8_t		onboard_sens_err_cnt = 0;
static 	uint8_t		sens_err_cnt = 0;

/*-------------------------- INIT FUNTION --------------------------*/

void Sensor_ADC_Init(void){
	TC1047_Init(&Temp_LV_Channel, ADC_TEMP_HANDLE, ADC_TEMP_50V_CHANNEL,3300);
	sens_init_status.tc1047_hv = SENS_INIT_OK;
	TC1047_Init(&Temp_HV_Channel, ADC_TEMP_HANDLE, ADC_TEMP_300V_CHANNEL,3300);
	sens_init_status.tc1047_lv = SENS_INIT_OK;
}


void  Sensor_I2C_Init(void) {
	uint8_t init_ret = 0;

	I2C_BusRecovery(&onboard_sensor_i2c,ONBOARD_SENSOR_I2C_PORT, ONBOARD_SENSOR_I2C_SCL, ONBOARD_SENSOR_I2C_PORT, ONBOARD_SENSOR_I2C_SDA);
	I2C_BusRecovery(&sensor_i2c, SENSOR_I2C_PORT_SCL, SENSOR_I2C_SCL, SENSOR_I2C_PORT_SDA, SENSOR_I2C_SDA);

	LL_mDelay(200);

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


void Sensor_I2C_task(void* arg) {
    uint8_t ret = I2C_Success;

    switch (sens_read_state) {
        case SENS_STATE_READ_LSM6DSOX:
            if (sens_init_status.lsm6d != SENS_INIT_FAIL) {
                ret = LSM6DSOX_Read_Data(&lsm6dsox_dev);
                if (ret != I2C_Success) {
                    onboard_sens_err_cnt++;
                    if (onboard_sens_err_cnt >= 4) {
                        sens_read_state = SENS_STATE_RECOVER_ONBOARD;
                        break;
                    }
                }
                else {
                    onboard_sens_err_cnt = 0;
                }
            }
            sens_read_state = SENS_STATE_READ_BMP390;
            break;

        case SENS_STATE_READ_BMP390:
            if (sens_init_status.bmp390 != SENS_INIT_FAIL) {
                ret = bmp390_temp_press_update(&bmp390_dev);
                if (ret != I2C_Success) {
                    sens_err_cnt++;
                    if (sens_err_cnt >= 2) {
                        sens_read_state = SENS_STATE_RECOVER;
                        break;
                    }
                }
                else {
                    sens_err_cnt = 0;
                }
            }
            sens_read_state = SENS_STATE_READ_H3LIS331DL;
            break;

        case SENS_STATE_READ_H3LIS331DL:
            if (sens_init_status.h3lis != SENS_INIT_FAIL) {
                ret = H3LIS331DL_Get_Accel(&h3lis_dev);
                if (ret != I2C_Success) {
                    onboard_sens_err_cnt++;
                    if (onboard_sens_err_cnt >= 4) {
                        sens_read_state = SENS_STATE_RECOVER_ONBOARD;
                        break;
                    }
                }
                else {
                    onboard_sens_err_cnt = 0;
                }
            }
            sens_read_state = SENS_STATE_READ_LSM6DSOX;
            break;
        case SENS_STATE_RECOVER_ONBOARD:
            I2C_BusRecovery(&onboard_sensor_i2c, ONBOARD_SENSOR_I2C_PORT, ONBOARD_SENSOR_I2C_SCL, ONBOARD_SENSOR_I2C_PORT, ONBOARD_SENSOR_I2C_SDA);
            I2C1_Init_Bridge();
            onboard_sens_err_cnt = 0;

            sens_read_state = SENS_STATE_READ_BMP390;
            break;
        case SENS_STATE_RECOVER:
            I2C_BusRecovery(&sensor_i2c, SENSOR_I2C_PORT_SCL, SENSOR_I2C_SCL, SENSOR_I2C_PORT_SDA, SENSOR_I2C_SDA);
            I2C3_Init_Bridge();
            sens_err_cnt = 0;

            sens_read_state = SENS_STATE_READ_H3LIS331DL;
            break;

        default:
            sens_read_state = SENS_STATE_READ_LSM6DSOX;
            break;
    }
}

/*-------------------------- ISR FUNTION --------------------------*/
void Sensor_I2C_EV_ISR(void){
	I2C_EV_IRQHandler(&sensor_i2c);
}


void Onboard_Sensor_I2C_EV_ISR(void){
	I2C_EV_IRQHandler(&onboard_sensor_i2c);
}

void Sensor_I2C_ER_ISR(void){
	I2C_ER_IRQHandler(&sensor_i2c);
}

void Onboard_Sensor_I2C_ER_ISR(void){
	I2C_ER_IRQHandler(&onboard_sensor_i2c);
}


