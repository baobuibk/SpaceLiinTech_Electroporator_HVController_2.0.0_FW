/*
 * fsp_frame.h
 *
 *  Created on: May 8, 2026
 *      Author: PV
 */

#ifndef FSP_LINE_TASK_FSP_FRAME_H_
#define FSP_LINE_TASK_FSP_FRAME_H_

#include "fsp.h"

#include "stdint.h"
#include "stdbool.h"

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
	FSP_CMD_GET_CAP_IS_CHARGING,
	FSP_CMD_GET_CAP_FINISH_CHARGE,
	FSP_CMD_GET_CAP_FINISH_DISCHARGE,
	FSP_CMD_GET_OVV_FLAG,

	FSP_CMD_MEASURE_VOLT,

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


typedef struct _GET_CHARGE_FINISH{

	uint8_t 	HV_charge_finish_flag;
	uint8_t 	LV_charge_finish_flag;

}GET_CHARGE_FINISH;

typedef struct _GET_DISCHARGE_FINISH{

	uint8_t 	HV_discharge_finish_flag;
	uint8_t 	LV_discharge_finish_flag;

}GET_DISCHARGE_FINISH;

typedef struct _GET_CAP_IS_CHARGING{

	uint8_t 	HV_is_charging_flag;
	uint8_t 	LV_is_charging_flag;

}GET_CAP_IS_CHARGING;

typedef struct _GET_OVV_FLAG_ {

	uint8_t		HV_OVV_flag;
	uint8_t		LV_OVV_flag;

}GET_OVV_FLAG;

typedef struct _CAP_GET_ALL{

	uint8_t		HV_Volt_High;
	uint8_t		HV_Volt_Low;
	uint8_t		LV_Volt_High;
	uint8_t 	LV_Volt_Low;

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

}GET_CAP_RELEASE;

typedef struct _GET_CAP_VOLT
{
	uint8_t		HV_Volt_High;
	uint8_t		HV_Volt_Low;
	uint8_t		LV_Volt_High;
	uint8_t 	LV_Volt_Low;

}GET_CAP_VOLT;

typedef struct _MEASURE_VOLT_
{
	uint8_t 	HV_raw_volt_high;
	uint8_t 	HV_raw_volt_low;
	uint8_t 	LV_raw_volt_high;
	uint8_t 	LV_raw_volt_low;

}MEASURE_VOLT;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Ultility Command ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
typedef struct _FSP_HANDSAKE_
{
	uint8_t 	Check;

} FSP_HANDSAKE;

typedef union _FSP_RESPONSE_
{
	bool	response;

} FSP_RESPONSE;


typedef union _FSP_Payload_Frame_typedef_
{
	SET_VOLT_ALL 			set_volt_all;
	SET_VOLT_HV				set_volt_hv;
	SET_VOLT_LV				set_volt_lv;
	SET_CHARGE				set_charge;
	SET_DISCHARGE			set_discharge;

	GET_CHARGE_FINISH		finish_charge_flag;
	GET_DISCHARGE_FINISH 	finish_discharge_flag;
	GET_CAP_IS_CHARGING		cap_is_charging_flag;
	GET_OVV_FLAG			get_ovv_flag;
	GET_CAP_ALL				get_cap_all;
	GET_CAP_VOLT			get_cap_volt;
	GET_CAP_CONTROL			get_cap_control;
	GET_CAP_RELEASE			get_cap_release;

	MEASURE_VOLT 			measure_volt;

	FSP_HANDSAKE			handshake;
	FSP_RESPONSE			fsp_response;

} FSP_Payload_Frame_typedef;

typedef struct _FSP_Payload_typedef_
{
	FSP_CMD_typedef				CMD;
	FSP_Payload_Frame_typedef 	Payload;

} FSP_Payload;


uint8_t FSP_Line_Process(void);


#endif /* FSP_LINE_TASK_FSP_FRAME_H_ */
