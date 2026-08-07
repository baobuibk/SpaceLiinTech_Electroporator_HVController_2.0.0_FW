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
Sens_Charger_Read_State_t	sens_charger_read_state;

I2C_Handle_t 		onboard_sensor_i2c;
I2C_Handle_t		sensor_i2c;
I2C_Handle_t		charger_sens_i2c;

LSM6DSOX_Handle_t 	lsm6dsox_dev;
BMP390_Handle_t		bmp390_dev;
H3LIS331DL_Handle_t	h3lis_dev;
ADS1115_Handle_t	ads1115_dev;

TC1047_Handle_t 	Temp_HV_Channel;
TC1047_Handle_t 	Temp_LV_Channel;

static 	uint8_t		onboard_sens_err_cnt = 0;
static 	uint8_t		sens_err_cnt = 0;
static 	uint8_t		charger_sens_err_cnt = 0;

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

void Sensor_Charger_I2C_Init(void){
	I2C_BusRecovery(&sensor_i2c, CHARGER_SENSOR_I2C_PORT, CHARGER_SENSOR_I2C_SCL, CHARGER_SENSOR_I2C_PORT, CHARGER_SENSOR_I2C_SDA);
	I2C_Init(&charger_sens_i2c,CHARGER_I2C_HANDLE);

	LL_mDelay(200);

	I2C_Status_t init_ret = I2C_Error;
	init_ret = ADS1115_Init(&ads1115_dev, &charger_sens_i2c);
	if(init_ret == I2C_Success) sens_init_status.ads1115 = SENS_INIT_OK;
	else sens_init_status.ads1115 = SENS_INIT_FAIL;

}

/*-------------------------- TASK FUNTION --------------------------*/
void Sensor_ADC_task(void*){
	Temp_HV_Channel.temp_value = TC1047_GetTemperature(&Temp_HV_Channel);
	Temp_LV_Channel.temp_value = TC1047_GetTemperature(&Temp_LV_Channel);
}


void Sensor_I2C_task(void*) {
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

void Sensor_Charger_I2C_Task(void *) {
	if (sens_init_status.ads1115 == SENS_INIT_FAIL) {
		return;
	}

	I2C_Status_t ret = I2C_Success;

	switch (sens_charger_read_state) {
	case SENS_CHARGER_STATE_READ_BAT:
		ret = ADS1115_SetConfig(&ads1115_dev, ADS1115_MUX_AIN0_GND, ADS1115_PGA_4_096V, ADS1115_MODE_SINGLE_SHOT,ADS1115_DR_128SPS);
		if (ret == I2C_Success) {
			ret = ADS1115_StartSingleConversion(&ads1115_dev);
		}

		if (ret != I2C_Success) {
			charger_sens_err_cnt++;
			if (charger_sens_err_cnt >= 2)
				sens_charger_read_state = SENS_CHARGER_STATE_EROR;
		} else {
			charger_sens_err_cnt = 0;
			sens_charger_read_state = SENS_CHARGER_STATE_PROCESS_BAT;
		}
		break;

	case SENS_CHARGER_STATE_PROCESS_BAT:
		float temp = 0;
		ret = ADS1115_ReadVoltage(&ads1115_dev, &temp);
		ads1115_dev.dev_volt_val[0] = (temp/1000.0f) * 4.33333;


		if (ret != I2C_Success) {
			charger_sens_err_cnt++;
			if (charger_sens_err_cnt >= 2)
				sens_charger_read_state = SENS_CHARGER_STATE_EROR;
		} else {
			charger_sens_err_cnt = 0;
			sens_charger_read_state = SENS_CHARGER_STATE_READ_AC;
		}
		break;

	case SENS_CHARGER_STATE_READ_AC:
		ret = ADS1115_SetConfig(&ads1115_dev, ADS1115_MUX_AIN1_GND, ADS1115_PGA_6_144V, ADS1115_MODE_SINGLE_SHOT, ADS1115_DR_128SPS);
		if (ret == I2C_Success)
			ret = ADS1115_StartSingleConversion(&ads1115_dev);

		if (ret != I2C_Success) {
			charger_sens_err_cnt++;
			if (charger_sens_err_cnt >= 2)
				sens_charger_read_state = SENS_CHARGER_STATE_EROR;
		} else {
			charger_sens_err_cnt = 0;
			sens_charger_read_state = SENS_CHARGER_STATE_PROCESS_AC;
		}
		break;

	case SENS_CHARGER_STATE_PROCESS_AC:
		ret = ADS1115_ReadVoltage(&ads1115_dev, &ads1115_dev.dev_volt_val[1]);

		if (ret != I2C_Success) {
			charger_sens_err_cnt++;
			if (charger_sens_err_cnt >= 2)
				sens_charger_read_state = SENS_CHARGER_STATE_EROR;
		} else {
			charger_sens_err_cnt = 0;
			sens_charger_read_state = SENS_CHARGER_STATE_READ_CHAR;
		}
		break;

	case SENS_CHARGER_STATE_READ_CHAR:
		ret = ADS1115_SetConfig(&ads1115_dev, ADS1115_MUX_AIN2_GND, ADS1115_PGA_6_144V, ADS1115_MODE_SINGLE_SHOT, ADS1115_DR_128SPS);
		if (ret == I2C_Success)
			ret = ADS1115_StartSingleConversion(&ads1115_dev);

		if (ret != I2C_Success) {
			charger_sens_err_cnt++;
			if (charger_sens_err_cnt >= 2)
				sens_charger_read_state = SENS_CHARGER_STATE_EROR;
		} else {
			charger_sens_err_cnt = 0;
			sens_charger_read_state = SENS_CHARGER_STATE_PROCESS_CHAR;
		}
		break;

	case SENS_CHARGER_STATE_PROCESS_CHAR:
		ret = ADS1115_ReadVoltage(&ads1115_dev, &ads1115_dev.dev_volt_val[2]);

		if (ret != I2C_Success) {
			charger_sens_err_cnt++;
			if (charger_sens_err_cnt >= 2)
				sens_charger_read_state = SENS_CHARGER_STATE_EROR;
		} else {
			charger_sens_err_cnt = 0;
			sens_charger_read_state = SENS_CHARGER_STATE_READ_BAT;
		}
		break;

	case SENS_CHARGER_STATE_EROR:

		I2C_BusRecovery(&charger_sens_i2c, CHARGER_SENSOR_I2C_PORT,
				CHARGER_SENSOR_I2C_SCL, CHARGER_SENSOR_I2C_PORT,
				CHARGER_SENSOR_I2C_SDA);
		I2C2_Init_Bridge();

		charger_sens_err_cnt = 0;
		sens_charger_read_state = SENS_CHARGER_STATE_READ_BAT;
		break;
	}
}

/*-------------------------- GET DATA FUNTION --------------------------*/












