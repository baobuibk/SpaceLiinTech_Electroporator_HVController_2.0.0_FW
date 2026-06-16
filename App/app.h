/*
 * app.h
 *
 *  Created on: Apr 22, 2026
 *      Author: PV
 */

#ifndef APP_H_
#define APP_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define MY_CORE_H_
#include "stm32f4_header.h"

// SYSTEM DRIVER //
#include "board.h"


// USER DRIVER //
#include <scheduler/scheduler.h>
#include "ring_buffer.h"
#include "uart_driver.h"
#include <cli/CLI_Src/embedded_cli.h>
#include <cli/CLI_Setup/cli_setup.h>
#include "cli_command.h"
//#include "spi.h"


// USER DIVICE //
//#include "MB85RS2MT.h"

// INCLUDE TASK //
#include "debug_cmd_task.h"
#include "cap_controller_task.h"
#include "fsp_line_task.h"


typedef enum
{
    CAP_CONTROLLER_CHARGE_TASK,
	CAP_CONTROLLER_MONITOR_TASK,
	CALIB_TASK,
	DEBUG_CMD_TASK,
    STATUS_LED,
} Task_List_t;

void App_Main(void);


#endif /* APP_H_ */
