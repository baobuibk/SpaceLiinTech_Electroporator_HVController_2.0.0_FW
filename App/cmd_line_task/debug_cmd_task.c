/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <cli/CLI_Setup/cli_setup.h>
#include <cli/CLI_Src/embedded_cli.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "cli_command.h"
#include "debug_cmd_task.h"


#include "app.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


UART_Driver_t*		CMD_line_handle;
UART_Driver_t  		DEBUG_UART;
RingBufElement      g_DEBUG_UART_TX_buffer[1024];
RingBufElement      g_DEBUG_UART_RX_buffer[4096];


CLI_UINT            g_DEBUG_CMD_line_buffer[BYTES_TO_CLI_UINTS(CLI_BUFFER_SIZE)];
EmbeddedCli 		*p_DEBUG_CLI = NULL;



void CMD_Line_Task_Init(void){

	UART_Driver_Init(&DEBUG_UART, "DEBUG_UART", DEBUG_UART_HANDLE, DEBUG_UART_IRQ,
					g_DEBUG_UART_TX_buffer, g_DEBUG_UART_RX_buffer,
					sizeof(g_DEBUG_UART_TX_buffer),sizeof(g_DEBUG_UART_RX_buffer));

	p_DEBUG_CLI = SystemCLI_Init(&DEBUG_UART, g_DEBUG_CMD_line_buffer, sizeof(g_DEBUG_CMD_line_buffer));

	UART_Driver_SendString(&DEBUG_UART, "HELLO\n\r");

	CMD_line_handle = &DEBUG_UART;

}

void Debug_CMD_Line_Task(void*){

	char rxData;

	while (UART_Driver_IsDataAvailable(&DEBUG_UART)) {

		rxData = UART_Driver_Read(&DEBUG_UART);
		embeddedCliReceiveChar(p_DEBUG_CLI, (char)rxData);

	}
	embeddedCliProcess(p_DEBUG_CLI);
}



