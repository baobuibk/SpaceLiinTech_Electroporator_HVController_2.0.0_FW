/*
 * sensor_charge_task.c
 *
 *  Created on: May 11, 2026
 *      Author: PV
 */

#include "sensor_charge_task.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "board.h"
#include "i2c.h"
#include "ADS1115.h"

#define NUM_OF_INPUT	3

ADS_1115_handle g_bat = {
    .config = {
        .input_channel = ADS1115_MUX_AIN0,
        .pga          = ADS1115_PGA_TWO,   /* ±4V cho cảm biến áp suất */
        .data_rate    = ADS1115_DATA_RATE_128,
    }
};

ADS_1115_handle g_ac_det = {
	 .config = {
	     .input_channel = ADS1115_MUX_AIN1,
	     .pga          = ADS1115_PGA_TWO,   /* ±4V cho cảm biến áp suất */
	     .data_rate    = ADS1115_DATA_RATE_128,
	 }
};

ADS_1115_handle g_char_det = {
	 .config = {
	     .input_channel = ADS1115_MUX_AIN2,
	     .pga          = ADS1115_PGA_TWO,   /* ±4V cho cảm biến áp suất */
	     .data_rate    = ADS1115_DATA_RATE_128,
	 },
};


i2c_stdio_typedef 	Charger_I2C;

I2C_data_t 		  	g_sensor_charge_I2C_data_array[16];

sensor_charge_request_rb_t Sensor_ADS115_rb;

ADS_1115_handle*	Input_list [NUM_OF_INPUT] = {
	&g_bat,
	&g_char_det,
	&g_ac_det,
};



void Sensor_charge_init(void){

	I2C_Init(&Charger_I2C, CHARGER_I2C_HANDLE, CHARGER_SENSOR_I2C_IRQ, g_sensor_charge_I2C_data_array, 16);

	ADS1115_Init_Handle(&g_bat);
	ADS1115_Init_Handle(&g_ac_det);
	ADS1115_Init_Handle(&g_char_det);

	Sensor_ADS115_rb.p_i2c = &Charger_I2C;
	Sensor_ADS115_rb.input_num = NUM_OF_INPUT;
	Sensor_ADS115_rb.is_complete = false;

	Sensor_ADS115_rb.current_state = SC_TASK_READ_BAT;
	memset(&Sensor_ADS115_rb.safe_data, 0, sizeof(sensor_charge_data_t));

}

void Sensor_charge_read_task(void* param){
    i2c_result_t result;

    switch (Sensor_ADS115_rb.current_state)
    {
        case SC_TASK_READ_BAT:
            result = ADS1115_Read_Value(&Charger_I2C, Input_list[0]);
            if (result == I2C_OK)
            {
                Sensor_ADS115_rb.safe_data.bat_voltage_mV = Input_list[0]->data.voltage_mV;
                Sensor_ADS115_rb.current_state = SC_TASK_READ_AC_DET;

            }
            else if (result != I2C_IS_RUNNING) {

                Sensor_ADS115_rb.current_state = SC_TASK_READ_AC_DET;
            }
            break;

        case SC_TASK_READ_AC_DET:
            result = ADS1115_Read_Value(&Charger_I2C, Input_list[1]);
            if (result == I2C_OK)
            {
                Sensor_ADS115_rb.safe_data.ac_det_voltage_mV = Input_list[1]->data.voltage_mV;
                Sensor_ADS115_rb.current_state = SC_TASK_READ_CHAR_DET;

            }
            else if (result != I2C_IS_RUNNING) {
                Sensor_ADS115_rb.current_state = SC_TASK_READ_CHAR_DET;
            }
            break;

        case SC_TASK_READ_CHAR_DET:
            result = ADS1115_Read_Value(&Charger_I2C, Input_list[2]);
            if (result == I2C_OK)
            {

                Sensor_ADS115_rb.safe_data.char_det_voltage_mV = Input_list[2]->data.voltage_mV;
                Sensor_ADS115_rb.safe_data.is_data_valid = true;

                Sensor_ADS115_rb.current_state = SC_TASK_READ_BAT;

            }
            else if (result != I2C_IS_RUNNING)
            {
                Sensor_ADS115_rb.current_state = SC_TASK_READ_BAT;
            }
            break;

        default:
            Sensor_ADS115_rb.current_state = SC_TASK_READ_BAT;
            break;
    }
}










