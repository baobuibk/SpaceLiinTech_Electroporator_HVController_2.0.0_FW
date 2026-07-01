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
#include "board.h"


typedef struct Sens_List_Status {

	uint8_t lsm6d;
    uint8_t h3lis;
    uint8_t	bmp390;

} Sens_List_Status_t;


extern LSM6DSOX_Data_t LSM6DSOX_Data;
extern H3LIS331DL_Data_t H3LIS331DL_Data;
extern BMP390_Value BMP390_Val;


void Sensor_I2C_Init(void);
void Sensor_I2C_task(void*);

#endif /* SENSOR_TASK_SENSOR_TASK_H_ */
