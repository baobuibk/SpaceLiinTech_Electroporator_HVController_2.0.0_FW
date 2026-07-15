/*
 * auto_pulsing_task.c
 *
 *  Created on: Jul 9, 2026
 *      Author: PV
 */

#include "auto_pulsing_task.h"
#include "h3lis331dl.h"

#include "fsp_frame.h"
#include "fsp_line_task.h"
#include "crc.h"


static void fsp_print(uint8_t packet_length);

Auto_Pulsing_Mode AP_Mode = AP_MODE_OFF;

/*--------------------- H3LIS ISR FUNCTION ----------------------*/

void Auto_Pulsing_Trigger_ISR_Handle(void){
	if(AP_Mode == AP_MODE_ON){

		ps_FSP_TX -> CMD = FSP_CMD_AUTO_ACCEL_TRIGGER;
		fsp_print(1);

		AP_Mode = AP_MODE_OFF;
		H3LIS331DL_Enable_INT1_All(&h3lis_dev,false);
	}
}

/*---------------------- STATIC FUNCTION -----------------------*/
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




