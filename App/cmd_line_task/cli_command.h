/*
 * cli_command.h
 *
 *  Created on: Apr 1, 2025
 *      Author: HTSANG
 */

#ifndef CLI_COMMAND_H_
#define CLI_COMMAND_H_

#include <cli/CLI_Setup/cli_setup.h>
#include <cli/CLI_Src/embedded_cli.h>
#include "main.h"

#include <stdio.h>

//void CLI_Command_Init(USART_TypeDef *handle_uart);
//void CLI_Command_Task_Update(void *pvParam);

const CliCommandBinding *getCliStaticBindings(void) ;
uint16_t getCliStaticBindingCount(void);

#endif /* CLI_COMMAND_H_ */
