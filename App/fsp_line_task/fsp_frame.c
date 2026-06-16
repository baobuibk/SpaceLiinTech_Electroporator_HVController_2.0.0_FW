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

#include "fsp_frame.h"
#include "db_cap_controller.h"



static void fsp_print(uint8_t packet_length);


uint8_t FSP_Line_Process(void)
{
	switch(ps_FSP_RX -> CMD)
	{
	case FSP_CMD_SET_CAP_VOLT_ALL:
	{
		uint16_t hv_set_volt = 	(ps_FSP_RX -> Payload.set_volt_all.HV_High << 8)
								|(ps_FSP_RX -> Payload.set_volt_all.HV_Low);
		uint16_t lv_set_volt =	(ps_FSP_RX -> Payload.set_volt_all.LV_High << 8)
								|(ps_FSP_RX -> Payload.set_volt_all.LV_Low);

		Cap_Set_Volt_All(hv_set_volt, lv_set_volt);

		return 1;
	}

	case FSP_CMD_SET_CAP_VOLT_HV:
	{
		uint16_t hv_set_volt = 	(ps_FSP_RX -> Payload.set_volt_hv.HV_High << 8)
								|(ps_FSP_RX -> Payload.set_volt_hv.HV_Low);

		Cap_Set_Volt(CAP_PRF_HV, hv_set_volt);

		return 1;
	}
	case FSP_CMD_SET_CAP_VOLT_LV:
	{
		uint16_t lv_set_volt = 	(ps_FSP_RX -> Payload.set_volt_lv.LV_High << 8)
								|(ps_FSP_RX -> Payload.set_volt_lv.LV_Low);

		Cap_Set_Volt(CAP_PRF_LV, lv_set_volt);

		return 1;
	}
	case FSP_CMD_SET_CAP_CONTROL:
	{
		bool hv_cmd_charge = (ps_FSP_RX -> Payload.set_charge.HV_cmd_charge);
		bool lv_cmd_charge = (ps_FSP_RX -> Payload.set_charge.LV_cmd_charge);

		Cap_Set_Charge_All(hv_cmd_charge, hv_cmd_charge, true, true);

		return 1;
	}
	case FSP_CMD_SET_CAP_RELEASE:
	{
		uint8_t hv_cmd_discharge = (ps_FSP_RX -> Payload.set_discharge.HV_cmd_discharge);
		uint8_t lv_cmd_discharge = (ps_FSP_RX -> Payload.set_discharge.LV_cmd_discharge);

		Cap_Set_Discharge_All(hv_cmd_discharge, lv_cmd_discharge);

		return 1;
	}
	case FSP_CMD_GET_OVV_FLAG:
	{
		uint8_t hv_ovv_flag;
		uint8_t lv_ovv_flag;

		db_cap_read(DB_ID_CAP_HV_OVV_FLAG, &hv_ovv_flag);
		db_cap_read(DB_ID_CAP_LV_OVV_FLAG, &lv_ovv_flag);

		ps_FSP_TX -> CMD = FSP_CMD_GET_OVV_FLAG;
		ps_FSP_TX -> Payload.get_ovv_flag.HV_OVV_flag = hv_ovv_flag;
		ps_FSP_TX -> Payload.get_ovv_flag.LV_OVV_flag = lv_ovv_flag;

		fsp_print(3);

		return 1;
	}
	case FSP_CMD_GET_CAP_ALL:
	{
		uint16_t hv_set_volt, lv_set_volt;
		uint8_t	hv_cmd_charge, hv_cmd_discharge;
		uint8_t	lv_cmd_charge, lv_cmd_discharge;

		hv_set_volt = Cap_Get_Set_Volt(CAP_PRF_HV);
		lv_set_volt = Cap_Get_Set_Volt(CAP_PRF_LV);

		hv_cmd_charge = Cap_Get_is_Charge(CAP_PRF_HV);
		hv_cmd_discharge = Cap_Get_is_Discharge(CAP_PRF_HV);

		lv_cmd_charge = Cap_Get_is_Charge(CAP_PRF_LV);
		lv_cmd_discharge = Cap_Get_is_Discharge(CAP_PRF_LV);

		ps_FSP_TX -> CMD = FSP_CMD_GET_CAP_ALL;
		ps_FSP_TX -> Payload.get_cap_all.HV_Volt_High = (uint8_t)(hv_set_volt >> 8);
		ps_FSP_TX -> Payload.get_cap_all.HV_Volt_Low  = (uint8_t)(hv_set_volt & 0xFF);

		ps_FSP_TX -> Payload.get_cap_all.LV_Volt_High = (uint8_t)(hv_set_volt >> 8);
		ps_FSP_TX -> Payload.get_cap_all.Lv_Volt_Low = (uint8_t)(hv_set_volt & 0xFF);

		ps_FSP_TX -> Payload.get_cap_all.HV_Volt_Charge = hv_cmd_charge;
		ps_FSP_TX -> Payload.get_cap_all.HV_Volt_Discharge = hv_cmd_discharge;

		ps_FSP_TX -> Payload.get_cap_all.LV_Volt_Charge = lv_cmd_charge;
		ps_FSP_TX -> Payload.get_cap_all.LV_Volt_Discharge = lv_cmd_discharge;

		fsp_print(9);
	}
	case FSP_CMD_GET_CAP_CONTROL:
	{
		uint8_t	hv_cmd_charge, lv_cmd_charge;

		hv_cmd_charge = Cap_Get_is_Charge(CAP_PRF_HV);
		lv_cmd_charge = Cap_Get_is_Charge(CAP_PRF_LV);

		ps_FSP_TX -> CMD = FSP_CMD_GET_CAP_CONTROL;
		ps_FSP_TX -> Payload.get_cap_control.HV_Volt_Charge = hv_cmd_charge;
		ps_FSP_TX -> Payload.get_cap_control.LV_Volt_Charge = lv_cmd_charge;

		fsp_print(3);
	}
	case FSP_CMD_GET_CAP_RELEASE:
	{
		uint8_t	hv_cmd_discharge, lv_cmd_discharge;

		hv_cmd_discharge = Cap_Get_is_Discharge(CAP_PRF_HV);
		lv_cmd_discharge = Cap_Get_is_Discharge(CAP_PRF_LV);

		ps_FSP_TX -> CMD = FSP_CMD_GET_CAP_CONTROL;
		ps_FSP_TX -> Payload.get_cap_release.HV_Volt_Discharge = hv_cmd_discharge;
		ps_FSP_TX -> Payload.get_cap_release.LV_Volt_Discharge = lv_cmd_discharge;

		fsp_print(3);
	}
	default:
		return 0;

	}
}


static void fsp_print(uint8_t packet_length)
{
	s_FSP_TX_Packet.sod 		= FSP_PKT_SOD;
	s_FSP_TX_Packet.src_adr 	= fsp_my_adr;
	s_FSP_TX_Packet.dst_adr 	= FSP_ADR_GPP;
	s_FSP_TX_Packet.length 		= packet_length;
	s_FSP_TX_Packet.type 		= FSP_PKT_TYPE_CMD_W_DATA;
	s_FSP_TX_Packet.eof 		= FSP_PKT_EOF;
	s_FSP_TX_Packet.crc16 		= crc16_CCITT(FSP_CRC16_INITIAL_VALUE, &s_FSP_TX_Packet.src_adr, s_FSP_TX_Packet.length + 4);

	uint8_t encoded_frame[25] = { 0 };
	uint8_t frame_len;
	fsp_encode(&s_FSP_TX_Packet, encoded_frame, &frame_len);

	UART_Driver_SendFSP(&GPP_UART, (char*)encoded_frame , frame_len);
}

