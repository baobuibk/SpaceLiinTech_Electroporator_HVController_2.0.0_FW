	/*
 * fsp_frame.c
 *
 *  Created on: May 8, 2026
 *      Author: PV
 */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "app.h"

#include "crc.h"
#include "fsp_frame.h"
#include "db_cap_controller.h"
#include "uart_driver.h"



static void fsp_print(uint8_t packet_length);


uint8_t FSP_Line_Process(void)
{
	switch(ps_FSP_RX -> CMD)
	{
	case FSP_CMD_SET_CAP_VOLT_ALL:
	{
		bool hv_cmd_charge, lv_cmd_charge = false;

		db_cap_read(DB_ID_CAP_HV_CMD_CHARGE, &hv_cmd_charge);
		db_cap_read(DB_ID_CAP_LV_CMD_CHARGE, &lv_cmd_charge);

		if(hv_cmd_charge || lv_cmd_charge)
		{
			ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_VOLT_ALL;
			ps_FSP_TX -> Payload.fsp_response.response = false;
			fsp_print(2);

			return 0;
		}

		uint16_t hv_set_volt = 	(ps_FSP_RX -> Payload.set_volt_all.HV_High << 8)
								|(ps_FSP_RX -> Payload.set_volt_all.HV_Low);
		uint16_t lv_set_volt =	(ps_FSP_RX -> Payload.set_volt_all.LV_High << 8)
								|(ps_FSP_RX -> Payload.set_volt_all.LV_Low);

		Cap_Set_Volt_All(hv_set_volt, lv_set_volt);

		ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_VOLT_ALL;
		ps_FSP_TX -> Payload.fsp_response.response = true;
		fsp_print(2);

		UART_Driver_SendString(&DEBUG_UART,"\r\nHV and LV CAP SET VOLT RECEIVED");

		return 1;
	}

	case FSP_CMD_SET_CAP_VOLT_HV:
	{
		bool hv_cmd_charge = false;
		db_cap_read(DB_ID_CAP_HV_CMD_CHARGE, &hv_cmd_charge);

		if(hv_cmd_charge){
			ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_VOLT_HV;
			ps_FSP_TX -> Payload.fsp_response.response = false;
			fsp_print(2);

			return 0;
		}

		uint16_t hv_set_volt = 	(ps_FSP_RX -> Payload.set_volt_hv.HV_High << 8)
								|(ps_FSP_RX -> Payload.set_volt_hv.HV_Low);

		Cap_Set_Volt(CAP_PRF_HV, hv_set_volt);

		ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_VOLT_HV;
		ps_FSP_TX -> Payload.fsp_response.response = true;
		fsp_print(2);
		
		UART_Driver_SendString(&DEBUG_UART,"\r\nHV CAP SET VOLT RECEIVED");

		return 1;
	}

	case FSP_CMD_SET_CAP_VOLT_LV:
	{
		bool lv_cmd_charge = false;
		db_cap_read(DB_ID_CAP_LV_CMD_CHARGE, &lv_cmd_charge);

		if(lv_cmd_charge){
			ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_VOLT_LV;
			ps_FSP_TX -> Payload.fsp_response.response = false;
			fsp_print(2);

			return 0;
		}

		uint16_t lv_set_volt = 	(ps_FSP_RX -> Payload.set_volt_lv.LV_High << 8)
								|(ps_FSP_RX -> Payload.set_volt_lv.LV_Low);

		Cap_Set_Volt(CAP_PRF_LV, lv_set_volt);

		ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_VOLT_LV;
		ps_FSP_TX -> Payload.fsp_response.response = true;
		fsp_print(2);

		UART_Driver_SendString(&DEBUG_UART,"\r\nLV CAP SET VOLT RECEIVED");

		return 1;
	}

	case FSP_CMD_SET_CAP_CONTROL:
	{

		uint16_t hv_set_volt = 0;
		uint16_t lv_set_volt = 0;
		
		db_cap_read(DB_ID_CAP_HV_VOLT_SET, &hv_set_volt);
		db_cap_read(DB_ID_CAP_LV_VOLT_SET, &lv_set_volt);

		uint16_t hv_raw_volt = Cap_Measure_Volt(CAP_PRF_HV);
		uint16_t lv_raw_volt = Cap_Measure_Volt(CAP_PRF_LV);

		if((hv_set_volt <= hv_raw_volt) || (lv_set_volt <= lv_raw_volt))
		{
			ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_CONTROL;
			ps_FSP_TX -> Payload.fsp_response.response = false;
			fsp_print(2);

			return 0;
		}

		bool hv_cmd_charge = (ps_FSP_RX -> Payload.set_charge.HV_cmd_charge);
		bool lv_cmd_charge = (ps_FSP_RX -> Payload.set_charge.LV_cmd_charge);

		Cap_Set_Charge_All(hv_cmd_charge, lv_cmd_charge, true, true);

		ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_CONTROL;
		ps_FSP_TX -> Payload.fsp_response.response = true;
		fsp_print(2);

		UART_Driver_SendString(&DEBUG_UART,"\r\nCAP SET CHARGE RECEIVED");

		return 1;
	}
	
	case FSP_CMD_SET_CAP_RELEASE:
	{
		uint8_t hv_cmd_discharge = (ps_FSP_RX -> Payload.set_discharge.HV_cmd_discharge);
		uint8_t lv_cmd_discharge = (ps_FSP_RX -> Payload.set_discharge.LV_cmd_discharge);

		Cap_Set_Discharge_All(hv_cmd_discharge, lv_cmd_discharge);

		ps_FSP_TX -> CMD = FSP_CMD_SET_CAP_RELEASE;
		ps_FSP_TX -> Payload.fsp_response.response =true;
		fsp_print(2);

		UART_Driver_SendString(&DEBUG_UART,"\r\nCAP SET DISCHARGE RECEIVED");

		return 1;
	}
	case FSP_CMD_RESET_CAP_OVV:
	{
		bool hv_ovv_flag = ps_FSP_RX -> Payload.reset_ovv_flag.HV_OVV_flag;
		bool lv_ovv_flag = ps_FSP_RX -> Payload.reset_ovv_flag.LV_OVV_flag;

		if (hv_ovv_flag == false) db_cap_write(DB_ID_CAP_HV_OVV_FLAG, &hv_ovv_flag);

		if (lv_ovv_flag == false)db_cap_write(DB_ID_CAP_LV_OVV_FLAG, &lv_ovv_flag);

		return 1;
	}
	case FSP_CMD_GET_OVV_FLAG:
	{
		bool hv_ovv_flag;
		bool lv_ovv_flag;

		db_cap_read(DB_ID_CAP_HV_OVV_FLAG, &hv_ovv_flag);
		db_cap_read(DB_ID_CAP_LV_OVV_FLAG, &lv_ovv_flag);

		ps_FSP_TX -> CMD = FSP_CMD_GET_OVV_FLAG;
		ps_FSP_TX -> Payload.get_ovv_flag.HV_OVV_flag = (uint8_t) hv_ovv_flag;
		ps_FSP_TX -> Payload.get_ovv_flag.LV_OVV_flag = (uint8_t) lv_ovv_flag;

		fsp_print(3);

		return 1;
	}
	
	case FSP_CMD_GET_CAP_ALL:
	{
		uint16_t hv_set_volt, lv_set_volt = 0;
		bool	hv_cmd_charge, hv_cmd_discharge = false;
		bool	lv_cmd_charge, lv_cmd_discharge = false;

		hv_set_volt = Cap_Get_Set_Volt(CAP_PRF_HV);
		lv_set_volt = Cap_Get_Set_Volt(CAP_PRF_LV);

		hv_cmd_charge = Cap_Get_is_Charge(CAP_PRF_HV);
		hv_cmd_discharge = Cap_Get_is_Discharge(CAP_PRF_HV);

		lv_cmd_charge = Cap_Get_is_Charge(CAP_PRF_LV);
		lv_cmd_discharge = Cap_Get_is_Discharge(CAP_PRF_LV);

		ps_FSP_TX -> CMD = FSP_CMD_GET_CAP_ALL;
		ps_FSP_TX -> Payload.get_cap_all.HV_Volt_High = (uint8_t)(hv_set_volt >> 8);
		ps_FSP_TX -> Payload.get_cap_all.HV_Volt_Low  = (uint8_t)(hv_set_volt & 0xFF);

		ps_FSP_TX -> Payload.get_cap_all.LV_Volt_High = (uint8_t)(lv_set_volt >> 8);
		ps_FSP_TX -> Payload.get_cap_all.LV_Volt_Low = (uint8_t)(lv_set_volt & 0xFF);

		ps_FSP_TX -> Payload.get_cap_all.HV_Volt_Charge =(uint8_t) hv_cmd_charge;
		ps_FSP_TX -> Payload.get_cap_all.HV_Volt_Discharge = (uint8_t) hv_cmd_discharge;

		ps_FSP_TX -> Payload.get_cap_all.LV_Volt_Charge = (uint8_t) lv_cmd_charge;
		ps_FSP_TX -> Payload.get_cap_all.LV_Volt_Discharge = (uint8_t) lv_cmd_discharge;

		fsp_print(9);
		
		return 1;
	}
	case FSP_CMD_GET_CAP_CONTROL:
	{
		bool	hv_cmd_charge, lv_cmd_charge;

		hv_cmd_charge = Cap_Get_is_Charge(CAP_PRF_HV);
		lv_cmd_charge = Cap_Get_is_Charge(CAP_PRF_LV);

		ps_FSP_TX -> CMD = FSP_CMD_GET_CAP_CONTROL;
		ps_FSP_TX -> Payload.get_cap_control.HV_Volt_Charge = (uint8_t) hv_cmd_charge;
		ps_FSP_TX -> Payload.get_cap_control.LV_Volt_Charge = (uint8_t) lv_cmd_charge;

		fsp_print(3);

		return 1;
	}
	case FSP_CMD_GET_CAP_RELEASE:
	{
		bool	hv_cmd_discharge, lv_cmd_discharge;

		hv_cmd_discharge = Cap_Get_is_Discharge(CAP_PRF_HV);
		lv_cmd_discharge = Cap_Get_is_Discharge(CAP_PRF_LV);

		ps_FSP_TX -> CMD = FSP_CMD_GET_CAP_RELEASE;
		ps_FSP_TX -> Payload.get_cap_release.HV_Volt_Discharge = (uint8_t) hv_cmd_discharge;
		ps_FSP_TX -> Payload.get_cap_release.LV_Volt_Discharge = (uint8_t) lv_cmd_discharge;

		fsp_print(3);

		return 1;
	}
	case FSP_CMD_GET_CAP_VOLT:
	{
		uint16_t hv_set_volt, lv_set_volt = 0;

		hv_set_volt = Cap_Get_Set_Volt(CAP_PRF_HV);
		lv_set_volt = Cap_Get_Set_Volt(CAP_PRF_LV);

		ps_FSP_TX -> CMD = FSP_CMD_GET_CAP_VOLT;
		ps_FSP_TX -> Payload.get_cap_volt.HV_Volt_High = (uint8_t)(hv_set_volt >> 8);
		ps_FSP_TX -> Payload.get_cap_volt.HV_Volt_Low  = (uint8_t)(hv_set_volt & 0xFF);

		ps_FSP_TX -> Payload.get_cap_volt.LV_Volt_High = (uint8_t)(lv_set_volt >> 8);
		ps_FSP_TX -> Payload.get_cap_volt.LV_Volt_Low = (uint8_t)(lv_set_volt & 0xFF);

		fsp_print(5);

		return 1;
	}
	case FSP_CMD_MEASURE_VOLT:
	{
		float hv_raw_mvolt = Cap_Measure_Volt(CAP_PRF_HV) * 1000.0f;
		float lv_raw_mvolt = Cap_Measure_Volt(CAP_PRF_LV) * 1000.0f;
		uint32_t hv_volt = (uint32_t)hv_raw_mvolt;
		uint16_t lv_volt = (uint16_t)lv_raw_mvolt;

		ps_FSP_TX -> CMD = FSP_CMD_MEASURE_VOLT;
		ps_FSP_TX -> Payload.measure_volt.HV_raw_volt[0] = (uint8_t)(hv_volt & 0xFF);
		ps_FSP_TX -> Payload.measure_volt.HV_raw_volt[1] = (uint8_t)((hv_volt >> 8) & 0xFF);
		ps_FSP_TX -> Payload.measure_volt.HV_raw_volt[2] = (uint8_t)((hv_volt >> 16) & 0xFF);
		ps_FSP_TX -> Payload.measure_volt.HV_raw_volt[3] = (uint8_t)((hv_volt >> 24) & 0xFF);
		ps_FSP_TX -> Payload.measure_volt.LV_raw_volt[0] = (uint8_t)(lv_volt & 0xFF);
		ps_FSP_TX -> Payload.measure_volt.LV_raw_volt[1] = (uint8_t)((lv_volt >> 8) & 0xFF);

		fsp_print(7);

		UART_Driver_SendString(&DEBUG_UART,"\r\nCAP MEASURE VOLT");

		return 1;

	}
	case FSP_CMD_GET_SENSOR_GYRO:
	{
//		 LSM6DSOX_Read_Data(&LSM6DSOX_Data);

		int32_t gyro_x = (int32_t)(lsm6dsox_dev.Data.Gyro.x * 1000);
		int32_t gyro_y = (int32_t)(lsm6dsox_dev.Data.Gyro.y * 1000);
		int32_t gyro_z = (int32_t)(lsm6dsox_dev.Data.Gyro.z * 1000);

		ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_GYRO;
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_x[0] = (uint8_t)(gyro_x & 0xFF);       
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_x[1] = (uint8_t)((gyro_x >> 8) & 0xFF);  
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_x[2] = (uint8_t)((gyro_x >> 16) & 0xFF); 
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_x[3] = (uint8_t)((gyro_x >> 24) & 0xFF);

		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_y[0] = (uint8_t)(gyro_y & 0xFF);       
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_y[1] = (uint8_t)((gyro_y >> 8) & 0xFF);  
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_y[2] = (uint8_t)((gyro_y >> 16) & 0xFF); 
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_y[3] = (uint8_t)((gyro_y >> 24) & 0xFF);

		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_z[0] = (uint8_t)(gyro_z & 0xFF);       
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_z[1] = (uint8_t)((gyro_z >> 8) & 0xFF);  
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_z[2] = (uint8_t)((gyro_z >> 16) & 0xFF); 
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_z[3] = (uint8_t)((gyro_z >> 24) & 0xFF);

		fsp_print(13);
	
		break;
	}
	case FSP_CMD_GET_SENSOR_ACCEL:
	{
//		 LSM6DSOX_Read_Data(&LSM6DSOX_Data);

		int32_t accel_x = (int32_t)(lsm6dsox_dev.Data.Accel.x * 1000);
		int32_t accel_y = (int32_t)(lsm6dsox_dev.Data.Accel.y * 1000);
		int32_t accel_z = (int32_t)(lsm6dsox_dev.Data.Accel.z * 1000);

		ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_ACCEL;
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_x[0] = (uint8_t)(accel_x & 0xFF);       
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_x[1] = (uint8_t)((accel_x >> 8) & 0xFF);  
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_x[2] = (uint8_t)((accel_x >> 16) & 0xFF); 
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_x[3] = (uint8_t)((accel_x >> 24) & 0xFF);

		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_y[0] = (uint8_t)(accel_y & 0xFF);       
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_y[1] = (uint8_t)((accel_y >> 8) & 0xFF);  
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_y[2] = (uint8_t)((accel_y >> 16) & 0xFF); 
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_y[3] = (uint8_t)((accel_y >> 24) & 0xFF);

		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_z[0] = (uint8_t)(accel_z & 0xFF);       
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_z[1] = (uint8_t)((accel_z >> 8) & 0xFF);  
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_z[2] = (uint8_t)((accel_z >> 16) & 0xFF); 
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_z[3] = (uint8_t)((accel_z >> 24) & 0xFF);

		fsp_print(25);

		break;
	}
	case FSP_CMD_GET_SENSOR_LSM6DSOX:
	{
//		 LSM6DSOX_Read_Data(&LSM6DSOX_Data);

		int32_t gyro_x = (int32_t)(lsm6dsox_dev.Data.Gyro.x * 1000);
		int32_t gyro_y = (int32_t)(lsm6dsox_dev.Data.Gyro.y * 1000);
		int32_t gyro_z = (int32_t)(lsm6dsox_dev.Data.Gyro.z * 1000);

		int32_t accel_x = (int32_t)(lsm6dsox_dev.Data.Accel.x * 1000);
		int32_t accel_y = (int32_t)(lsm6dsox_dev.Data.Accel.y * 1000);
		int32_t accel_z = (int32_t)(lsm6dsox_dev.Data.Accel.z * 1000);

		ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_LSM6DSOX;

		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_x[0] = (uint8_t)(gyro_x & 0xFF);       
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_x[1] = (uint8_t)((gyro_x >> 8) & 0xFF);  
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_x[2] = (uint8_t)((gyro_x >> 16) & 0xFF); 
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_x[3] = (uint8_t)((gyro_x >> 24) & 0xFF);

		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_y[0] = (uint8_t)(gyro_y & 0xFF);       
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_y[1] = (uint8_t)((gyro_y >> 8) & 0xFF);  
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_y[2] = (uint8_t)((gyro_y >> 16) & 0xFF); 
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_y[3] = (uint8_t)((gyro_y >> 24) & 0xFF);

		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_z[0] = (uint8_t)(gyro_z & 0xFF);       
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_z[1] = (uint8_t)((gyro_z >> 8) & 0xFF);  
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_z[2] = (uint8_t)((gyro_z >> 16) & 0xFF); 
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Gyro_z[3] = (uint8_t)((gyro_z >> 24) & 0xFF);

		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_x[0] = (uint8_t)(accel_x & 0xFF);       
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_x[1] = (uint8_t)((accel_x >> 8) & 0xFF);  
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_x[2] = (uint8_t)((accel_x >> 16) & 0xFF); 
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_x[3] = (uint8_t)((accel_x >> 24) & 0xFF);

		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_y[0] = (uint8_t)(accel_y & 0xFF);       
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_y[1] = (uint8_t)((accel_y >> 8) & 0xFF);  
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_y[2] = (uint8_t)((accel_y >> 16) & 0xFF); 
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_y[3] = (uint8_t)((accel_y >> 24) & 0xFF);

		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_z[0] = (uint8_t)(accel_z & 0xFF);       
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_z[1] = (uint8_t)((accel_z >> 8) & 0xFF);  
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_z[2] = (uint8_t)((accel_z >> 16) & 0xFF); 
		ps_FSP_TX->Payload.get_sensor_lsm6dsox.Accel_z[3] = (uint8_t)((accel_z >> 24) & 0xFF);

		fsp_print(25);

		break;
	}
	case FSP_CMD_GET_SENSOR_TEMP:
	{
//		 bmp390_temp_press_update(&BMP390_Val);

		int32_t temp 	= (int32_t)(bmp390_dev.temperature * 1000);

		ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_TEMP;

		ps_FSP_TX->Payload.get_sensor_bmp390.Temp[0] = (uint8_t)(temp & 0xFF);
		ps_FSP_TX->Payload.get_sensor_bmp390.Temp[1] = (uint8_t)((temp >> 8) & 0xFF);
		ps_FSP_TX->Payload.get_sensor_bmp390.Temp[2] = (uint8_t)((temp >> 16) & 0xFF);
		ps_FSP_TX->Payload.get_sensor_bmp390.Temp[3] = (uint8_t)((temp >> 24) & 0xFF);

		fsp_print(13);
		break;
	}
	case FSP_CMD_GET_SENSOR_PRESSURE:
	{
//		 bmp390_temp_press_update(&BMP390_Val);

		int32_t pressure = (int32_t)(bmp390_dev.pressure  * 1000);

		ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_PRESSURE;

		ps_FSP_TX->Payload.get_sensor_bmp390.Pressure[0] = (uint8_t)(pressure & 0xFF);
		ps_FSP_TX->Payload.get_sensor_bmp390.Pressure[1] = (uint8_t)((pressure >> 8) & 0xFF);
		ps_FSP_TX->Payload.get_sensor_bmp390.Pressure[2] = (uint8_t)((pressure >> 16) & 0xFF);
		ps_FSP_TX->Payload.get_sensor_bmp390.Pressure[3] = (uint8_t)((pressure >> 24) & 0xFF);

		fsp_print(13);
		break;
	}
	case FSP_CMD_GET_SENSOR_ALTITUDE:
	{
//		 bmp390_temp_press_update(&BMP390_Val);

		int32_t altitude = (int32_t)(bmp390_dev.altitude  * 1000);

		ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_ALTITUDE;

		ps_FSP_TX->Payload.get_sensor_bmp390.Altitude[0] = (uint8_t)(altitude & 0xFF);
		ps_FSP_TX->Payload.get_sensor_bmp390.Altitude[1] = (uint8_t)((altitude >> 8) & 0xFF);
		ps_FSP_TX->Payload.get_sensor_bmp390.Altitude[2] = (uint8_t)((altitude >> 16) & 0xFF);
		ps_FSP_TX->Payload.get_sensor_bmp390.Altitude[3] = (uint8_t)((altitude >> 24) & 0xFF);

		fsp_print(13);
		break;
	}
	case FSP_CMD_GET_SENSOR_BMP390:
	{
//		 bmp390_temp_press_update(&BMP390_Val);
		int32_t temp 	= (int32_t)(bmp390_dev.temperature * 1000);
		int32_t pressure = (int32_t)(bmp390_dev.pressure  * 1000);
		int32_t altitude = (int32_t)(bmp390_dev.altitude  * 1000);

		ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_BMP390;

		ps_FSP_TX->Payload.get_sensor_bmp390.Temp[0] = (uint8_t)(temp & 0xFF);
		ps_FSP_TX->Payload.get_sensor_bmp390.Temp[1] = (uint8_t)((temp >> 8) & 0xFF);
		ps_FSP_TX->Payload.get_sensor_bmp390.Temp[2] = (uint8_t)((temp >> 16) & 0xFF);
		ps_FSP_TX->Payload.get_sensor_bmp390.Temp[3] = (uint8_t)((temp >> 24) & 0xFF);

		ps_FSP_TX->Payload.get_sensor_bmp390.Pressure[0] = (uint8_t)(pressure & 0xFF);
		ps_FSP_TX->Payload.get_sensor_bmp390.Pressure[1] = (uint8_t)((pressure >> 8) & 0xFF);
		ps_FSP_TX->Payload.get_sensor_bmp390.Pressure[2] = (uint8_t)((pressure >> 16) & 0xFF);
		ps_FSP_TX->Payload.get_sensor_bmp390.Pressure[3] = (uint8_t)((pressure >> 24) & 0xFF);

		ps_FSP_TX->Payload.get_sensor_bmp390.Altitude[0] = (uint8_t)(altitude & 0xFF);
		ps_FSP_TX->Payload.get_sensor_bmp390.Altitude[1] = (uint8_t)((altitude >> 8) & 0xFF);
		ps_FSP_TX->Payload.get_sensor_bmp390.Altitude[2] = (uint8_t)((altitude >> 16) & 0xFF);
		ps_FSP_TX->Payload.get_sensor_bmp390.Altitude[3] = (uint8_t)((altitude >> 24) & 0xFF);

		fsp_print(13);

		break;
	}
	case FSP_CMD_GET_SENSOR_H3LIS331DL:
	{
//		 H3LIS331DL_Get_Accel(&H3LIS331DL_Data);

		int32_t accel_x = (int32_t)(h3lis_dev.Data.x * 1000);
		int32_t accel_y = (int32_t)(h3lis_dev.Data.y * 1000);
		int32_t accel_z = (int32_t)(h3lis_dev.Data.z * 1000);

		ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_H3LIS331DL;
		ps_FSP_TX->Payload.get_sensor_h3lis331dl.Accel_x[0] = (uint8_t)(accel_x & 0xFF);       
		ps_FSP_TX->Payload.get_sensor_h3lis331dl.Accel_x[1] = (uint8_t)((accel_x >> 8) & 0xFF);  
		ps_FSP_TX->Payload.get_sensor_h3lis331dl.Accel_x[2] = (uint8_t)((accel_x >> 16) & 0xFF); 
		ps_FSP_TX->Payload.get_sensor_h3lis331dl.Accel_x[3] = (uint8_t)((accel_x >> 24) & 0xFF);

		ps_FSP_TX->Payload.get_sensor_h3lis331dl.Accel_y[0] = (uint8_t)(accel_y & 0xFF);       
		ps_FSP_TX->Payload.get_sensor_h3lis331dl.Accel_y[1] = (uint8_t)((accel_y >> 8) & 0xFF);  
		ps_FSP_TX->Payload.get_sensor_h3lis331dl.Accel_y[2] = (uint8_t)((accel_y >> 16) & 0xFF); 
		ps_FSP_TX->Payload.get_sensor_h3lis331dl.Accel_y[3] = (uint8_t)((accel_y >> 24) & 0xFF);

		ps_FSP_TX->Payload.get_sensor_h3lis331dl.Accel_z[0] = (uint8_t)(accel_z & 0xFF);       
		ps_FSP_TX->Payload.get_sensor_h3lis331dl.Accel_z[1] = (uint8_t)((accel_z >> 8) & 0xFF);  
		ps_FSP_TX->Payload.get_sensor_h3lis331dl.Accel_z[2] = (uint8_t)((accel_z >> 16) & 0xFF); 
		ps_FSP_TX->Payload.get_sensor_h3lis331dl.Accel_z[3] = (uint8_t)((accel_z >> 24) & 0xFF);

		fsp_print(13);

		break;
	}
	case FSP_CMD_SET_SENSOR_H3LIS331DL_FS:
	{	
		uint8_t i2c_ret = 0;
		uint8_t fs_value = ps_FSP_RX -> Payload.set_sensor_h3lis331dl_fs.fs_value;

		if (fs_value == 1) {
			i2c_ret = H3LIS331DL_Set_FS(&h3lis_dev,H3LIS331DL_FS_100G);
		} 
		else if (fs_value == 2) 
		{
			i2c_ret = H3LIS331DL_Set_FS(&h3lis_dev,H3LIS331DL_FS_200G);
		} 
		else if (fs_value == 4) 
		{
			i2c_ret = H3LIS331DL_Set_FS(&h3lis_dev,H3LIS331DL_FS_400G);
		} 

		ps_FSP_TX -> CMD = FSP_CMD_SET_SENSOR_H3LIS331DL_FS;

		if(i2c_ret != I2C_Success) ps_FSP_TX -> Payload.set_sensor_h3lis331dl_fs.set_h3lis_fs_response = 0;
		else ps_FSP_TX -> Payload.set_sensor_h3lis331dl_fs.set_h3lis_fs_response  = 1;

		fsp_print(3);

		break;
	}
	case FSP_CMD_GET_SENSOR_H3LIS331DL_FS:
	{
		uint8_t i2c_ret = 0;
		H3LIS331DL_FS_t fs_value = 0;

		i2c_ret =H3LIS331DL_Get_FS(&h3lis_dev,&fs_value);

		ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_H3LIS331DL_FS;

		if(i2c_ret != I2C_Success) ps_FSP_TX -> Payload.get_sensor_h3lis331dl_fs.get_h3lis_fs_response = 0;
		else ps_FSP_TX -> Payload.get_sensor_h3lis331dl_fs.get_h3lis_fs_response = 1;

		if(fs_value == H3LIS331DL_FS_100G) {
			ps_FSP_TX -> Payload.get_sensor_h3lis331dl_fs.fs_value = 1;
		} 
		else if (fs_value == H3LIS331DL_FS_200G) 
		{
			ps_FSP_TX -> Payload.get_sensor_h3lis331dl_fs.fs_value = 2;
		} 
		else if (fs_value == H3LIS331DL_FS_400G) 
		{
			ps_FSP_TX -> Payload.get_sensor_h3lis331dl_fs.fs_value = 4;
		} 

		fsp_print(3);
		break;
	}
	case FSP_CMD_GET_SENSOR_HV_TEMP:
	{
		uint16_t hv_temp_ret = Temp_HV_Channel.temp_value * 100;

		ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_HV_TEMP;
		ps_FSP_TX -> Payload.get_sensor_hv_temp.HV_temp_high = (uint8_t)((hv_temp_ret >> 8) & 0xFF);
		ps_FSP_TX -> Payload.get_sensor_hv_temp.HV_temp_low = (uint8_t)(hv_temp_ret & 0xFF);

		fsp_print(3);

		break;
	}
	case FSP_CMD_GET_SENSOR_LV_TEMP:
	{
		uint16_t lv_temp_ret = Temp_LV_Channel.temp_value * 100;

		ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_LV_TEMP;
		ps_FSP_TX -> Payload.get_sensor_lv_temp.LV_temp_high = (uint8_t)((lv_temp_ret >> 8) & 0xFF);
		ps_FSP_TX -> Payload.get_sensor_lv_temp.LV_temp_low = (uint8_t)(lv_temp_ret & 0xFF);

		fsp_print(3);
		break;
	}
	case FSP_CMD_SET_THRESHOLD_ACCEL:
	{
		uint8_t	i2c_ret = 0;
		int32_t threshold_accel_mg = 	(int32_t)ps_FSP_RX->Payload.set_threshold_accel.threshold_total_mg[0]|
                              	  		((int32_t)ps_FSP_RX->Payload.set_threshold_accel.threshold_total_mg[1] << 8)|
								  		((int32_t)ps_FSP_RX->Payload.set_threshold_accel.threshold_total_mg[2] << 16)|
								  		((int32_t)ps_FSP_RX->Payload.set_threshold_accel.threshold_total_mg[3] << 24);

		float threshold_accel_g = (float)threshold_accel_mg / 1000; //convert to "g" 

		i2c_ret = H3LIS331DL_Set_Interrupt_Threshold(&h3lis_dev,threshold_accel_g);

		ps_FSP_TX -> CMD = FSP_CMD_SET_THRESHOLD_ACCEL;

		if(i2c_ret != I2C_Success) ps_FSP_TX -> Payload.set_threshold_accel.set_threshold_response = 0;
		else ps_FSP_TX -> Payload.set_threshold_accel.set_threshold_response = 1;

		fsp_print(18);

		break;
	}
	case FSP_CMD_GET_THRESHOLD_ACCEL:
	{
		uint8_t	i2c_ret = 0;
		float thresholde_accel_g = 0.0f;

		i2c_ret = H3LIS331DL_Get_Interrupt_Threshold(&h3lis_dev,&thresholde_accel_g);

		int32_t threshold_accel_mg = (int32_t)(thresholde_accel_g * 1000);

		ps_FSP_TX -> CMD = FSP_CMD_GET_THRESHOLD_ACCEL;

		if (i2c_ret != I2C_Success) {
			ps_FSP_TX->Payload.get_threshold_accel.get_threshold_response = 0;
		}
		else {
			ps_FSP_TX->Payload.get_threshold_accel.get_threshold_response = 1;
			ps_FSP_TX->Payload.get_threshold_accel.threshold_total_mg[0] = (uint8_t) (threshold_accel_mg & 0xFF);
			ps_FSP_TX->Payload.get_threshold_accel.threshold_total_mg[1] = (uint8_t) ((threshold_accel_mg >> 8) & 0xFF);
			ps_FSP_TX->Payload.get_threshold_accel.threshold_total_mg[2] = (uint8_t) ((threshold_accel_mg >> 16) & 0xFF);
			ps_FSP_TX->Payload.get_threshold_accel.threshold_total_mg[3] = (uint8_t) ((threshold_accel_mg >> 24) & 0xFF);
		}
		fsp_print(18);
		
		break;
	}
	case FSP_CMD_SET_AUTO_ACCEL:
	{
		uint8_t	i2c_ret = 0;
		bool auto_pulsing_en = ps_FSP_RX -> Payload.set_auto_accel.auto_accel_enable;

		if (auto_pulsing_en == true) AP_Mode = AP_MODE_ON;
		else if(auto_pulsing_en == false) AP_Mode = AP_MODE_OFF;

		i2c_ret = H3LIS331DL_Enable_INT1_All(&h3lis_dev,auto_pulsing_en);

		ps_FSP_TX -> CMD = FSP_CMD_SET_AUTO_ACCEL;
		if(i2c_ret != I2C_Success)
			ps_FSP_TX -> Payload.set_auto_accel.set_auto_accel_response = 0;
		else
			ps_FSP_TX -> Payload.set_auto_accel.set_auto_accel_response = 1;

		fsp_print(3);

		break;
	}
	case FSP_CMD_GET_SENSOR_HVC_INIT_STATE:
	{
		ps_FSP_TX -> CMD = FSP_CMD_GET_SENSOR_HVC_INIT_STATE;
		ps_FSP_TX -> Payload.get_sensor_hvc_init_state.init_state_bmp390 	= sens_init_status.bmp390;
		ps_FSP_TX -> Payload.get_sensor_hvc_init_state.init_state_lsm6d 	= sens_init_status.lsm6d;
		ps_FSP_TX -> Payload.get_sensor_hvc_init_state.init_state_h3lis 	= sens_init_status.h3lis;
		ps_FSP_TX -> Payload.get_sensor_hvc_init_state.init_state_tc1047_hv = sens_init_status.tc1047_hv;
		ps_FSP_TX -> Payload.get_sensor_hvc_init_state.init_state_tc1047_lv	= sens_init_status.tc1047_lv;
		ps_FSP_TX -> Payload.get_sensor_hvc_init_state.init_state_ads1115 	= sens_init_status.ads1115;

		fsp_print(7);
		break;
	}

	case FSP_CMD_GET_BAT_VOLT:
	{
		ps_FSP_TX -> CMD = FSP_CMD_GET_BAT_VOLT;
		ps_FSP_TX -> Payload.get_bat_volt.bat_volt = (uint8_t) (ads1115_dev.dev_volt_val[0] * 10.0f);
		fsp_print(2);
		break;
	}
	case FSP_CMD_RESET_HVC:
	{
		NVIC_SystemReset();
	}
	case FSP_CMD_HANDSHAKE:
	{
		ps_FSP_TX -> CMD = FSP_CMD_HANDSHAKE;
		fsp_print(1);
	}
	default:
		break;
	}
	return 1;
}

/*---------------------------------------------------- STATIC FUNCTION ---------------------------------------------------*/

static void fsp_print(uint8_t packet_length)
{
	s_FSP_TX_Packet.sod 		= FSP_PKT_SOD;
	s_FSP_TX_Packet.src_adr 	= fsp_my_adr;
	s_FSP_TX_Packet.dst_adr 	= FSP_ADR_GPP;
	s_FSP_TX_Packet.length 		= packet_length;
	s_FSP_TX_Packet.type 		= FSP_PKT_TYPE_CMD_W_DATA;
	s_FSP_TX_Packet.eof 		= FSP_PKT_EOF;
	s_FSP_TX_Packet.crc16 		= crc16_CCITT(FSP_CRC16_INITIAL_VALUE, &s_FSP_TX_Packet.src_adr, s_FSP_TX_Packet.length + 4);

	uint8_t encoded_frame[35] = { 0 };
	uint8_t frame_len;
	fsp_encode(&s_FSP_TX_Packet, encoded_frame, &frame_len);

	UART_Driver_SendFSP(&GPP_UART, (char*)encoded_frame , frame_len);
}

