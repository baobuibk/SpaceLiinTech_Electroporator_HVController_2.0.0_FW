/*
 * sensor_task.h
 *
 *  Created on: Jun 30, 2026
 *      Author: PV
 */

#ifndef SENSOR_TASK_SENSOR_TASK_H_
#define SENSOR_TASK_SENSOR_TASK_H_

#include "i2c.h"
#include "lsm6dsox.h"
#include "h3lis331dl.h"
#include "bmp390.h"
#include "TC1047.h"

#include "board.h"

typedef enum {
	SENS_INIT_FAIL = 0,
	SENS_INIT_OK
}Sens_Init_State_t;

typedef enum {
	SENS_STATE_READ_LSM6DSOX = 0,
	SENS_STATE_PROCESS_LSM6DSOX,
	SENS_STATE_READ_H3LIS331DL,
	SENS_STATE_PROCESS_H3LIS331DL,
	SENS_STATE_READ_BMP390,
	SENS_STATE_PROCESS_BMP390,
	SENS_STATE_REC_BUS,
}Sens_Read_State_t;


typedef struct Sens_List_Status {
	Sens_Init_State_t 	lsm6d;
	Sens_Init_State_t 	h3lis;
	Sens_Init_State_t	bmp390;
} Sens_List_Status_t;


extern LSM6DSOX_Handle_t 	lsm6dsox_dev;
extern BMP390_Handle_t		bmp390_dev;
extern H3LIS331DL_Handle_t	h3lis_dev;
extern TC1047_Handle_t Temp_HV_Channel;
extern TC1047_Handle_t Temp_LV_Channel;

void Sensor_I2C_Init(void);
void Sensor_ADC_Init(void);
void Sensor_I2C_task(void*);
void Sensor_ADC_task(void*);

void Sensor_I2C_EV_ISR(void);
void Onboard_Sensor_I2C_EV_ISR(void);
void Sensor_I2C_ER_ISR(void);
void Onboard_Sensor_I2C_ER_ISR(void);

#endif /* SENSOR_TASK_SENSOR_TASK_H_ */
