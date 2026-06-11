/*
 * sensor_charge_task.h
 *
 *  Created on: May 11, 2026
 *      Author: PV
 */

#ifndef SENSOR_TASK_SENSOR_CHARGE_TASK_H_
#define SENSOR_TASK_SENSOR_CHARGE_TASK_H_

#include "board.h"
#include "i2c.h"
#include "ADS1115.h"

typedef struct {
    int32_t bat_voltage_mV;
    int32_t ac_det_voltage_mV;
    int32_t char_det_voltage_mV;
    bool    is_data_valid;
} sensor_charge_data_t;


typedef enum {
    SC_TASK_READ_BAT,
    SC_TASK_READ_AC_DET,
    SC_TASK_READ_CHAR_DET,
    SC_TASK_WAIT
} sensor_charge_task_state_t;

typedef struct _sensor_charge_request_rb_t_
{
	i2c_stdio_typedef*		    p_i2c;
	uint8_t 				    input_num;
	i2c_result_t			    is_complete;
    sensor_charge_task_state_t  current_state;
    sensor_charge_data_t        safe_data;

} sensor_charge_request_rb_t;

extern sensor_charge_request_rb_t Sensor_ADS115_rb;

void Sensor_charge_init(void);
void Sensor_charge_read_task(void*);

#endif /* SENSOR_TASK_SENSOR_CHARGE_TASK_H_ */
