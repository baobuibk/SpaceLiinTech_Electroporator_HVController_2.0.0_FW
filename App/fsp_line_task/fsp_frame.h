/*
 * fsp_frame.h
 *
 *  Created on: May 8, 2026
 *      Author: PV
 */

#ifndef FSP_LINE_TASK_FSP_FRAME_H_
#define FSP_LINE_TASK_FSP_FRAME_H_

#include "fsp_line_task.h"

typedef enum _FSP_CMD_typedef_
{
	FSP_CMD_SET_CAP_VOLT_ALL = 0,
	FSP_CMD_SET_CAP_VOLT_HV,
	FSP_CMD_SET_CAP_VOLT_LV,
	FSP_CMD_SET_CAP_CONTROL,
	FSP_CMD_SET_CAP_RELEASE,

	FSP_CMD_GET_CAP_VOLT,
	FSP_CMD_GET_CAP_RELEASE,
	FSP_CMD_GET_CAP_CONTROL,
	FSP_CMD_GET_CAP_ALL,
	FSP_CMD_GET_OVV_FLAG,

	FSP_CMD_GET_SENSOR_GYRO,
	FSP_CMD_GET_SENSOR_ACCEL,
	FSP_CMD_GET_SENSOR_LSM6DSOX,

	FSP_CMD_GET_SENSOR_TEMP,
	FSP_CMD_GET_SENSOR_PRESSURE,
	FSP_CMD_GET_SENSOR_ALTITUDE,
	FSP_CMD_GET_SENSOR_BMP390,
	FSP_CMD_GET_SENSOR_H3LIS331DL,
	FSP_CMD_GET_SENSOR_H3LIS331DL_FS,
	FSP_CMD_SET_SENSOR_H3LIS331DL_FS,

	FSP_CMD_GET_SENSOR_ADS1115_BAT,
	FSP_CMD_GET_SENSOR_ADS1115_CHAR,
	FSP_CMD_GET_SENSOR_ADS1115_AC,

	FSP_CMD_GET_TEMP_HV,
	FSP_CMD_GET_TEMP_LV,

	FSP_CMD_HANDSHAKE,

} FSP_CMD_typedef;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Control Cap ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

typedef struct _SET_VOLT_HV_ {

	uint8_t		HV_High;
	uint8_t		HV_Low;

}SET_VOLT_HV;

typedef struct _SET_VOLT_LV_ {

	uint8_t		LV_High;
	uint8_t		LV_Low;

}SET_VOLT_LV;

typedef struct _SET_VOLT_ALL_{

	uint8_t		HV_High;
	uint8_t		HV_Low;
	uint8_t		LV_High;
	uint8_t		LV_Low;

}SET_VOLT_ALL;

typedef struct _SET_CHARGE_ {

	uint8_t		HV_cmd_charge;
	uint8_t		LV_cmd_charge;

}SET_CHARGE;

typedef struct _SET_DISCHARGE_ {

	uint8_t		HV_cmd_discharge;
	uint8_t		LV_cmd_discharge;

}SET_DISCHARGE;

typedef struct _GET_OVV_FLAG_ {

	uint8_t		HV_OVV_flag;
	uint8_t		LV_OVV_flag;

}GET_OVV_FLAG;

typedef struct _CAP_GET_ALL{

	uint8_t		HV_Volt_High;
	uint8_t		HV_Volt_Low;
	uint8_t		LV_Volt_High;
	uint8_t 	Lv_Volt_Low;

	uint8_t		HV_Volt_Charge;
	uint8_t		LV_Volt_Charge;

	uint8_t		HV_Volt_Discharge;
	uint8_t		LV_Volt_Discharge;

}GET_CAP_ALL;

typedef struct _GET_CAP_CONTROL
{
	uint8_t		HV_Volt_Charge;
	uint8_t		LV_Volt_Charge;

}GET_CAP_CONTROL;

typedef struct _GET_CAP_RELEASE
{
	uint8_t		HV_Volt_Discharge;
	uint8_t		LV_Volt_Discharge;

}GET_CAP_RELEASEL;




/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Sensor Command ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
typedef struct _GET_SENSOR_GYRO_
{
	uint8_t 	gyro_x[2];
	uint8_t 	gyro_y[2];
	uint8_t 	gyro_z[2];

} GET_SENSOR_GYRO;

typedef struct _GET_SENSOR_ACCEL_
{
	uint8_t 	accel_x[2];
	uint8_t 	accel_y[2];
	uint8_t 	accel_z[2];

} GET_SENSOR_ACCEL;

typedef struct _GET_SENSOR_LSM6DSOX_
{
	uint8_t 	gyro_x[2];
	uint8_t 	gyro_y[2];
	uint8_t 	gyro_z[2];
	uint8_t 	accel_x[2];
	uint8_t 	accel_y[2];
	uint8_t 	accel_z[2];

} GET_SENSOR_LSM6DSOX;

typedef struct _GET_SENSOR_TEMP_
{
	uint8_t 	temp[6];

} GET_SENSOR_TEMP;

typedef struct _GET_SENSOR_PRESSURE_
{
	uint8_t 	pressure[7];

} GET_SENSOR_PRESSURE;

typedef struct _GET_SENSOR_ALTITUDE_
{
	uint8_t 	altitude[4];

} GET_SENSOR_ALTITUDE;

typedef struct _GET_SENSOR_BMP390_
{
	uint8_t 	temp[6];
	uint8_t 	pressure[7];
	uint8_t 	altitude[4];

} GET_SENSOR_BMP390;

typedef struct _GET_SENSOR_H3LIS331DL_
{
	uint8_t 	accel_x[3];
	uint8_t 	accel_y[3];
	uint8_t 	accel_z[3];

} GET_SENSOR_H3LIS331DL;



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Ultility Command ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
typedef struct _FSP_HANDSAKE_
{
	uint8_t 	Check;

} FSP_HANDSAKE;

typedef union _FSP_Payload_Frame_typedef_
{
	SET_VOLT_ALL 		set_volt_all;
	SET_VOLT_HV			set_volt_hv;
	SET_VOLT_LV			set_volt_lv;

	SET_CHARGE			set_charge;
	SET_DISCHARGE		set_discharge;

	GET_OVV_FLAG		get_ovv_flag;

	GET_CAP_ALL			get_cap_all;
	GET_CAP_CONTROL		get_cap_control;
	GET_CAP_RELEASEL	get_cap_release;

	/* :::::::::: I2C Sensor Command :::::::: */
	GET_SENSOR_GYRO							get_sensor_gyro;
	GET_SENSOR_ACCEL						get_sensor_accel;
	GET_SENSOR_LSM6DSOX						get_sensor_LSM6DSOX;

	GET_SENSOR_TEMP							get_sensor_temp;
	GET_SENSOR_PRESSURE						get_sensor_pressure;
	GET_SENSOR_ALTITUDE						get_sensor_altitude;
	GET_SENSOR_BMP390						get_sensor_BMP390;

	GET_SENSOR_H3LIS331DL					get_sensor_H3LIS331DL;

} FSP_Payload_Frame_typedef;

typedef struct _FSP_Payload_typedef_
{
	uint8_t						CMD;
	FSP_Payload_Frame_typedef 	Payload;

} FSP_Payload;


uint8_t FSP_Line_Process(void);


#endif /* FSP_LINE_TASK_FSP_FRAME_H_ */
