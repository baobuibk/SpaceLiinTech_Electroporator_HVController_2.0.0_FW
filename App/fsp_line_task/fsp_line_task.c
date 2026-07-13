/*
 * fsp_line_task.c
 *
 *  Created on: May 8, 2026
 *      Author: PV
 */

#include "fsp.h"
#include "fsp_line_task.h"

#include <string.h>


typedef struct{

				uint16_t 	buffer_size;
				char 		*p_buffer;

	volatile 	uint16_t 	write_index;
	volatile 	char 		RX_char;

} fsp_line_typedef;


UART_Driver_t  		GPP_UART;
RingBufElement      g_GPP_UART_TX_buffer[GPP_TX_SIZE];
RingBufElement      g_GPP_UART_RX_buffer[GPP_RX_SIZE];

fsp_packet_t 		s_FSP_TX_Packet;
fsp_packet_t 		s_FSP_RX_Packet;
FSP_Payload 		*ps_FSP_TX = (FSP_Payload*) (&s_FSP_TX_Packet.payload);
FSP_Payload 		*ps_FSP_RX = (FSP_Payload*) (&s_FSP_RX_Packet.payload);

fsp_line_typedef 	FSP_line;
char 				g_FSP_line_buffer[FSP_BUF_LEN];

uint8_t				g_FSP_line_return = 1;

void FSP_Line_Task_Init(void)
{
	UART_Driver_Init(	&GPP_UART, "GPP_UART", GPP_UART_HANDLE, GPP_UART_IRQ,
						g_GPP_UART_TX_buffer, g_GPP_UART_RX_buffer,
						sizeof(g_GPP_UART_TX_buffer),sizeof(g_GPP_UART_RX_buffer));

	FSP_line.p_buffer 		= g_FSP_line_buffer;
	FSP_line.buffer_size 	= FSP_BUF_LEN;
	FSP_line.write_index 	= 0;

	if (FSP_line.buffer_size != 0)
	{
		memset((void*) FSP_line.p_buffer, 0, sizeof(FSP_line.p_buffer));
	}

	fsp_init(FSP_ADR_GPC);

}

int cntfsp = 0;
void FSP_Line_Task (void*)
{
	cntfsp++;
	uint8_t FSP_return, time_out = 0;

	for (time_out = 50; (UART_Driver_IsDataAvailable(&GPP_UART)) && (time_out != 0); time_out--)
	{
		FSP_line.RX_char = UART_Driver_Read(&GPP_UART);

		if (FSP_line.RX_char == FSP_PKT_SOD)
		{
			FSP_line.write_index = 0;
		}
		else if (FSP_line.RX_char == FSP_PKT_EOF)
		{
			FSP_return = frame_decode((uint8_t*) FSP_line.p_buffer, FSP_line.write_index, &s_FSP_RX_Packet);


			if (FSP_return == FSP_PKT_READY)
				FSP_Line_Process();

			FSP_line.write_index = 0;
		}
		else
		{
			FSP_line.p_buffer[FSP_line.write_index] = FSP_line.RX_char;
			FSP_line.write_index++;

			if (FSP_line.write_index > FSP_line.buffer_size)
				FSP_line.write_index = 0;

		}
	}
}



