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
#include "ADS1115.h"

#include "main.h"

#include "board.h"

typedef enum {
	SENS_INIT_FAIL = 0,
	SENS_INIT_OK
}Sens_Init_State_t;

typedef struct Sens_List_Status {
	Sens_Init_State_t 	lsm6d;
	Sens_Init_State_t 	h3lis;
	Sens_Init_State_t	bmp390;
	Sens_Init_State_t	tc1047_hv;
	Sens_Init_State_t	tc1047_lv;
	Sens_Init_State_t	ads1115;
} Sens_List_Status_t;

typedef enum {
	SENS_STATE_READ_LSM6DSOX = 0,
	SENS_STATE_READ_H3LIS331DL,
	SENS_STATE_READ_BMP390,
	SENS_STATE_RECOVER_ONBOARD,
	SENS_STATE_RECOVER,
}Sens_Read_State_t;

typedef enum {
	SENS_CHARGER_STATE_READ_BAT = 0,
	SENS_CHARGER_STATE_PROCESS_BAT,
	SENS_CHARGER_STATE_READ_AC,
	SENS_CHARGER_STATE_PROCESS_AC,
	SENS_CHARGER_STATE_READ_CHAR,
	SENS_CHARGER_STATE_PROCESS_CHAR,
	SENS_CHARGER_STATE_EROR,
}Sens_Charger_Read_State_t;



extern Sens_List_Status_t 	sens_init_status;

extern LSM6DSOX_Handle_t 	lsm6dsox_dev;
extern BMP390_Handle_t		bmp390_dev;
extern H3LIS331DL_Handle_t	h3lis_dev;
extern ADS1115_Handle_t		ads1115_dev;
extern TC1047_Handle_t 		Temp_HV_Channel;
extern TC1047_Handle_t 		Temp_LV_Channel;

void Sensor_I2C_Init(void);
void Sensor_ADC_Init(void);
void Sensor_Charger_I2C_Init(void);

void Sensor_I2C_task(void*);
void Sensor_ADC_task(void*);
void Sensor_Charger_I2C_Task(void *);



#endif /* SENSOR_TASK_SENSOR_TASK_H_ */
