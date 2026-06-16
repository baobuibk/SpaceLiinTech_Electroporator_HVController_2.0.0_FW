/*
 * fsp_line_task.h
 *
 *  Created on: May 8, 2026
 *      Author: PV
 */

#ifndef FSP_LINE_TASK_FSP_LINE_TASK_H_
#define FSP_LINE_TASK_FSP_LINE_TASK_H_

#include "app.h"

#include "fsp.h"
#include "fsp_frame.h"

extern UART_Driver_t 		GPP_UART;

extern fsp_packet_t		    s_FSP_RX_Packet;
extern fsp_packet_t		    s_FSP_TX_Packet;

extern FSP_Payload  	    *ps_FSP_RX;
extern FSP_Payload		    *ps_FSP_TX;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: FSP Line Task Init :::::::: */
void FSP_Line_Task_Init();

/* :::::::::: FSP Line Task ::::::::::::: */
void FSP_Line_Task(void*);


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */





#endif /* FSP_LINE_TASK_FSP_LINE_TASK_H_ */
