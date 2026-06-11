/*
 * debug_cmd_task.h
 *
 *  Created on: Apr 26, 2026
 *      Author: PV
 */

#ifndef CMD_LINE_TASK_DEBUG_CMD_TASK_H_
#define CMD_LINE_TASK_DEBUG_CMD_TASK_H_



extern UART_Driver_t*		CMD_line_handle;
extern UART_Driver_t  		DEBUG_UART;
extern EmbeddedCli 			*p_DEBUG_CLI ;
extern CLI_UINT            g_DEBUG_CMD_line_buffer[BYTES_TO_CLI_UINTS(CLI_BUFFER_SIZE)];


void CMD_Line_Task_Init(void);
void Debug_CMD_Line_Task(void*);


#endif /* CMD_LINE_TASK_DEBUG_CMD_TASK_H_ */
