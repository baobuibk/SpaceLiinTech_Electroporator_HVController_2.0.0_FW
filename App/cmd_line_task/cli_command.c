/*
 * cli_command.c
 *
 *  Created on: Apr 1, 2025
 *      Author: HTSANG
 */

#include <cli/CLI_Setup/cli_setup.h>
#include <cli/CLI_Src/embedded_cli.h>
#include "cli_command.h"

#include "stdlib.h"
#include "board.h"
#include "app.h"


/*************************************************
 *                Private variable                 *
 *************************************************/
//static uint8_t CMD_process_state = 0;
//static uint16_t current_h3lis_fs = 100;
static void double_to_string(double value, char *buffer, uint8_t precision);

/*************************************************
 *                Command Define                 *
 *************************************************/

static void CMD_ClearCLI(EmbeddedCli *cli, char *args, void *context);
static void CMD_Reset(EmbeddedCli *cli, char *args, void *context);

static void	CMD_SET_CAP_VOLT_ALL(EmbeddedCli *cli, char *args, void *context);
static void	CMD_SET_CAP_VOLT_HV (EmbeddedCli *cli, char *args, void *context);
static void CMD_SET_CAP_VOLT_LV (EmbeddedCli *cli, char *args, void *context);
static void CMD_SET_CAP_CONTROL (EmbeddedCli *cli, char *args, void *context);
static void CMD_SET_CAP_RELEASE (EmbeddedCli *cli, char *args, void *context);

static void CMD_GET_CAP_VOLT (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_CAP_CONTROL (EmbeddedCli *cli, char *args, void *context);
static void	CMD_GET_CAP_RELEASE (EmbeddedCli *cli, char *args, void *context);
static void CMD_GET_CAP_ALL (EmbeddedCli *cli, char *args, void *context);

//static void CMD_GET_SENSOR_GYRO(EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_SENSOR_ACCEL(EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_SENSOR_LSM6DSOX(EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_SENSOR_TEMP(EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_SENSOR_PRESSURE(EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_SENSOR_ALTITUDE(EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_SENSOR_BMP390(EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_SENSOR_H3LIS(EmbeddedCli *cli, char *args, void *context);
//static void CMD_SET_SENSOR_H3LIS_FS(EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_SENSOR_H3LIS_FS(EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_SENSOR_ADS1115_BAT(EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_SENSOR_ADS1115_AC(EmbeddedCli *cli, char *args, void *context);
//static void CMD_GET_SENSOR_ADS1115_CHAR(EmbeddedCli *cli, char *args, void *context);






/*************************************************
 *                 Command  Array                *
 *************************************************/
// Guide: Command bindings are declared in the following order:
// { category, name, help, tokenizeArgs, context, binding }
// - category: Command group; set to NULL if grouping is not needed.
// - name: Command name (required)
// - help: Help string describing the command (required)
// - tokenizeArgs: Set to true to automatically split arguments when the command is called.
// - context: Pointer to a command-specific context; can be NULL.
// - binding: Callback function that handles the command.

static const CliCommandBinding cliStaticBindings_internal[] = {
    { NULL, "help",         "format: help",                                         false, NULL, CMD_Help },
    { NULL, "reset",        "Reset MCU: reset",                                     false, NULL, CMD_Reset },             // Giữ từ ví dụ ban đầu
    { NULL, "clr",          "Clears the console",                                   false, NULL, CMD_ClearCLI },          // Giữ từ ví dụ ban đầu


	{ NULL,	"SET_CAP_VOLT_ALL", 	"format: SET_CAP_VOLT_ALL [HV_Volt] [LV_Volt]",		true,NULL,CMD_SET_CAP_VOLT_ALL},
	{ NULL,	"SET_CAP_VOLT_HV", 		"format: SET_CAP_VOLT_HV [HV_Volt]",				true,NULL,CMD_SET_CAP_VOLT_HV},
	{ NULL,	"SET_CAP_VOLT_LV", 		"format: SET_CAP_VOLT_LV [LV_Volt]",				true,NULL,CMD_SET_CAP_VOLT_LV},
	{ NULL,	"SET_CAP_CONTROL", 		"format: SET_CAP_CONTROL [N] [S]",					true,NULL,CMD_SET_CAP_CONTROL},
	{ NULL,	"SET_CAP_RELEASE", 		"format: SET_CAP_RELEASE [N] [S]",					true,NULL,CMD_SET_CAP_RELEASE},

	{ NULL,	"GET_CAP_VOLT", 		"format: GET_CAP_VOLT ",							false,NULL,CMD_GET_CAP_VOLT},
	{ NULL,	"GET_CAP_RELEASE", 		"format: GET_CAP_RELEASE",							false,NULL,CMD_GET_CAP_RELEASE},
	{ NULL,	"GET_CAP_CONTROL", 		"format: GET_CAP_CONTROL",							false,NULL,CMD_GET_CAP_CONTROL},
	{ NULL,	"GET_CAP_ALL", 			"format: GET_CAP_ALL",								false,NULL,CMD_GET_CAP_ALL},

//	{ NULL,	"GET_SENSOR_GYRO", 		"format: GET_SENSOR_GYRO",							false,NULL,CMD_GET_SENSOR_GYRO},
//	{ NULL,	"GET_SENSOR_ACCEL", 	"format: GET_SENSOR_ACCEL",							false,NULL,CMD_GET_SENSOR_ACCEL},
//	{ NULL,	"GET_SENSOR_LSM6DSOX", 	"format: GET_SENSOR_LSM6DSOX",						false,NULL,CMD_GET_SENSOR_LSM6DSOX},
//	{ NULL,	"GET_SENSOR_TEMP", 		"format: GET_SENSOR_TEMP",							false,NULL,CMD_GET_SENSOR_TEMP},
//	{ NULL,	"GET_SENSOR_PRESSURE", 	"format: GET_SENSOR_PRESSURE",						false,NULL,CMD_GET_SENSOR_PRESSURE},
//	{ NULL,	"GET_SENSOR_ALTITUDE", 	"format: GET_SENSOR_ALTITUDE",						false,NULL,CMD_GET_SENSOR_ALTITUDE},
//	{ NULL,	"GET_SENSOR_BMP390", 	"format: GET_SENSOR_BMP390",						false,NULL,CMD_GET_SENSOR_BMP390},
//	{ NULL,	"GET_SENSOR_H3LIS", 	"format: GET_SENSOR_H3LIS",							false,NULL,CMD_GET_SENSOR_H3LIS},
//	{ NULL,	"SET_SENSOR_H3LIS_FS", 	"format: SET_SENSOR_H3LIS_FS [X] ",					true,NULL,CMD_SET_SENSOR_H3LIS_FS},
//	{ NULL,	"GET_SENSOR_H3LIS_FS", 	"format: GET_SENSOR_H3LIS_FS",						false,NULL,CMD_GET_SENSOR_H3LIS_FS},
//
//	{ NULL,	"GET_SENSOR_ADS1115_BAT", 	"format: GET_SENSOR_ADS1115_BAT",				false,NULL,CMD_GET_SENSOR_ADS1115_BAT},
//	{ NULL,	"GET_SENSOR_ADS1115_CHAR", 	"format: GET_SENSOR_ADS1115_CHAR",				false,NULL,CMD_GET_SENSOR_ADS1115_CHAR},
//	{ NULL,	"GET_SENSOR_ADS1115_AC", 	"format: GET_SENSOR_ADS1115_AC",				false,NULL,CMD_GET_SENSOR_ADS1115_AC},


};

/*************************************************
 *             Command List Function             *
 *************************************************/


static void CMD_ClearCLI(EmbeddedCli *cli, char *args, void *context) {
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "\33[2J");
    embeddedCliPrint(cli, buffer);
}

static void CMD_Reset(EmbeddedCli *cli, char *args, void *context) {
	NVIC_SystemReset();
    embeddedCliPrint(cli, "");
}

static void	CMD_SET_CAP_VOLT_ALL(EmbeddedCli *cli, char *args, void *context){
	int argc = embeddedCliGetTokenCount(args);
	if(argc < 2) {
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_FEW_ARGS");
		return;
	}

	else if(argc > 2){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS\n\r");
		return;
	}

	int receive_argm[2];

	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	if ((receive_argm[0] > 350) || (receive_argm[0] < 0)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}
	else if ((receive_argm[1] > 60) || (receive_argm[1] < 0)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}

	uint8_t is_return = 0;

	if (Cap_Get_is_Charge(&g_Cap_300V) == true)
	{
		embeddedCliPrint(cli, "\n\r> HV CAP IS CHARGING, PLEASE DISABLE CHARGING BEFORE SET NEW VOLT");
		is_return = 1;
	}

	if (Cap_Get_is_Charge(&g_Cap_50V) == true)
	{
		embeddedCliPrint(cli, "\n\r> LV CAP IS CHARGING, PLEASE DISABLE CHARGING BEFORE SET NEW VOLT");
		is_return = 1;
	}

	if (is_return == 1)
	{
		return ;
	}

	Cap_Set_Volt_All(receive_argm[0], receive_argm[1], true, true);
	embeddedCliPrint(cli,"\n\r> CMDLINE_OK");
	return;

}
static void	CMD_SET_CAP_VOLT_HV (EmbeddedCli *cli, char *args, void *context){
	if (g_is_calib_running == true)
	{
		return ;
	}

	int argc = embeddedCliGetTokenCount(args);
	if(argc < 1) {
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_FEW_ARGS");
		return;
	}
	else if(argc > 1){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	int receive_argm;
	receive_argm = atoi(embeddedCliGetToken(args, 1));

	if ((receive_argm > 350) || (receive_argm < 0)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}

	if (Cap_Get_is_Charge(&g_Cap_300V) == true)
	{
		embeddedCliPrint(cli, "\n\r> HV CAP IS CHARGING, PLEASE DISABLE CHARGING BEFORE SET NEW VOLT");
		return ;
	}

	Cap_Set_Volt(&g_Cap_300V, receive_argm, true);
	embeddedCliPrint(cli,"\n\r> CMDLINE_OK");

	return;


}
static void CMD_SET_CAP_VOLT_LV (EmbeddedCli *cli, char *args, void *context){
	if (g_is_calib_running == true)
	{
		return ;
	}
	int argc = embeddedCliGetTokenCount(args);
	if(argc < 1) {
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_FEW_ARGS");
		return;
	}
	else if(argc > 1){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	int receive_argm;

	receive_argm = atoi(embeddedCliGetToken(args, 1));

	if ((receive_argm > 60) || (receive_argm < 0)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}

	if (Cap_Get_is_Charge(&g_Cap_50V) == true)
	{
		embeddedCliPrint(cli,"\n\r> LV CAP IS CHARGING, PLEASE DISABLE CHARGING BEFORE SET NEW VOLT");
		return ;
	}

	Cap_Set_Volt(&g_Cap_50V, receive_argm, true);
	embeddedCliPrint(cli,"\n\r> CMDLINE_OK");
	return;

}
static void CMD_SET_CAP_CONTROL (EmbeddedCli *cli, char *args, void *context){

	if (g_is_calib_running == true)
	{
		return ;
	}

	int argc = embeddedCliGetTokenCount(args);
	if(argc < 2) {
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_FEW_ARGS");
		return;
	}
	else if(argc > 2){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
		return;
	}


	int receive_argm[2];
	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	if ((receive_argm[0] > 1) || (receive_argm[0] < 0)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}
	else if ((receive_argm[1] > 1) || (receive_argm[1] < 0)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}

	uint16_t set_volt;
	uint16_t measure_volt;
	uint8_t  is_return = 0;

	if (receive_argm[0] == 1)
	{
		set_volt 	 = Cap_Get_Set_Volt(&g_Cap_300V);
		measure_volt = Cap_Measure_Volt(&g_Cap_300V);

		if (set_volt < measure_volt)
		{
			embeddedCliPrint(cli,"\n\r> CURRENT HV VOLT IS HIGHER THAN YOUR SET VOLT, PLEASE DISCHARGE BEFORE CHARGING");
			is_return = 1;
		}
	}

	if (receive_argm[1] == 1)
	{
		set_volt 	 = Cap_Get_Set_Volt(&g_Cap_50V);
		measure_volt = Cap_Measure_Volt(&g_Cap_50V);

		if (set_volt < measure_volt)
		{
			embeddedCliPrint(cli,"CURRENT LV VOLT IS HIGHER THAN YOUR SET VOLT, PLEASE DISCHARGE BEFORE CHARGING\n\r");
			is_return = 1;
		}
	}

	if (is_return == 1)
	{
		return ;
	}


	Cap_Set_Charge_All(receive_argm[0], receive_argm[1], true, true);
	embeddedCliPrint(cli,"\n\r> CMDLINE_OK");

	return ;
}

static void CMD_SET_CAP_RELEASE (EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);

	if(argc < 2) {
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_FEW_ARGS");
		return;
	}
	else if(argc > 2){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	int receive_argm[2];
	receive_argm[0] = atoi(embeddedCliGetToken(args, 1));
	receive_argm[1] = atoi(embeddedCliGetToken(args, 2));

	if ((receive_argm[0] > 1) || (receive_argm[0] < 0)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}
	else if ((receive_argm[1] > 1) || (receive_argm[1] < 0)){
		embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARG");
		return;
	}
	Cap_Set_Discharge_All(receive_argm[0], receive_argm[1], true, true);
	embeddedCliPrint(cli,"CMDLINE_OK\n\r");
	return ;

}
static void CMD_GET_CAP_VOLT (EmbeddedCli *cli, char *args, void *context){
	char msg [64];

	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	uint16_t hv_cap_set_voltage, lv_cap_set_voltage;
	hv_cap_set_voltage = Cap_Get_Set_Volt(&g_Cap_300V);
	lv_cap_set_voltage = Cap_Get_Set_Volt(&g_Cap_50V);

	sprintf(msg, "\n\r> HV CAP IS SET AT: %dV, LV CAP IS SET AT: %dV", hv_cap_set_voltage, lv_cap_set_voltage);
	embeddedCliPrint(cli,msg);

}
static void CMD_GET_CAP_CONTROL (EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
		return;
	}
	if (Cap_Get_is_Charge(&g_Cap_300V) == true)
	{
		embeddedCliPrint(cli, "\n\r> HV CAP IS CHARGING");
	}
	else
	{
		embeddedCliPrint(cli, "\n\r> HV CAP IS NOT CHARGING");
	}

	if (Cap_Get_is_Charge(&g_Cap_50V) == true)
	{
		embeddedCliPrint(cli,"\n\r> LV CAP IS CHARGING");
	}
	else
	{
		embeddedCliPrint(cli, "\n\r> LV CAP IS NOT CHARGING");
	}

}
static void	CMD_GET_CAP_RELEASE (EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	if (Cap_Get_is_Discharge(&g_Cap_300V) == true)
	{
		embeddedCliPrint(cli, "\n\r> HV CAP IS DISCHARGING");
	}
	else
	{
		embeddedCliPrint(cli, "\n\r> HV CAP IS NOT DISCHARGING");
	}

	if (Cap_Get_is_Discharge(&g_Cap_50V) == true)
	{
		embeddedCliPrint(cli, "\n\r> LV CAP IS DISCHARGING");
	}
	else
	{
		embeddedCliPrint(cli, "\n\r> LV CAP IS NOT DISCHARGING");
	}

}
static void CMD_GET_CAP_ALL (EmbeddedCli *cli, char *args, void *context){
	char msg[100];
	int argc = embeddedCliGetTokenCount(args);

	if(argc != 0){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
		return;
	}

	uint16_t hv_cap_set_voltage, lv_cap_set_voltage;

	hv_cap_set_voltage = Cap_Get_Set_Volt(&g_Cap_300V);
	lv_cap_set_voltage = Cap_Get_Set_Volt(&g_Cap_50V);

	sprintf(msg, "\n\r> HV CAP IS SET AT: %dV, LV CAP IS SET AT: %dV", hv_cap_set_voltage, lv_cap_set_voltage);
	embeddedCliPrint(cli,msg);

	if (Cap_Get_is_Charge(&g_Cap_300V) == true)
	{
		embeddedCliPrint(cli, "\n\r> HV CAP IS CHARGING");
	}
	else
	{
		embeddedCliPrint(cli, "\n\r> HV CAP IS NOT CHARGING");
	}

	if (Cap_Get_is_Charge(&g_Cap_50V) == true)
	{
		embeddedCliPrint(cli,"\n\r> LV CAP IS CHARGING");
	}
	else
	{
		embeddedCliPrint(cli, "\n\r> LV CAP IS NOT CHARGING");
	}

	if (Cap_Get_is_Discharge(&g_Cap_300V) == true)
	{
		embeddedCliPrint(cli, "\n\r> HV CAP IS DISCHARGING");
	}
	else
	{
		embeddedCliPrint(cli, "\n\r> HV CAP IS NOT DISCHARGING");
	}

	if (Cap_Get_is_Discharge(&g_Cap_50V) == true)
	{
		embeddedCliPrint(cli, "\n\r> LV CAP IS DISCHARGING");
	}
	else
	{
		embeddedCliPrint(cli, "\n\r> LV CAP IS NOT DISCHARGING");
	}

}

//static void CMD_GET_SENSOR_GYRO(EmbeddedCli *cli, char *args, void *context){
//
//	char msg[100];
//
//	switch (CMD_process_state)
//	{
//	case 0:
//	{
//		int argc = embeddedCliGetTokenCount(args);
//		if(argc != 0){
//			embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//			return;
//		}
//
//		Sensor_Read_Value(SENSOR_READ_GYRO);
//		CMD_process_state = 1;
//		return;
//	}
//	case 1:
//	{
//		i2c_result_t return_value = Is_Sensor_Read_Complete(&Sensor_LSM6DSOX_rb);
//
//		if (return_value == I2C_IS_RUNNING)
//		{
//			return ;
//		}
//		else if (return_value != I2C_OK)
//		{
//			CMD_process_state = 0;
//			return ;
//		}
//
//		sprintf(msg,"> GYRO x: %dmpds; GYRO y: %dmpds; GYRO z: %dmpds\n", Sensor_Gyro.x, Sensor_Gyro.y, Sensor_Gyro.z);
//		UART_Driver_SendString(CMD_line_handle,msg);
//		CMD_process_state = 0;
//	    return ;
//	}
//	default:
//		break;
//	} //switch
//
//	return;
//}
//static void CMD_GET_SENSOR_ACCEL(EmbeddedCli *cli, char *args, void *context){
//
//	char msg[100];
//	switch (CMD_process_state)
//	{
//	case 0:
//	{
//		int argc = embeddedCliGetTokenCount(args);
//		if(argc != 0){
//			embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//			return;
//		}
//		Sensor_Read_Value(SENSOR_READ_ACCEL);
//		CMD_process_state = 1;
//		return;
//	}
//	case 1:
//	{
//		i2c_result_t return_value = Is_Sensor_Read_Complete(&Sensor_LSM6DSOX_rb);
//
//		if (return_value == I2C_IS_RUNNING)
//		{
//			return;
//		}
//		else if (return_value != I2C_OK)
//		{
//			CMD_process_state = 0;
//			return;
//		}
//
//		sprintf(msg, "> ACCEL x: %dmg; ACCEL y: %dmg; ACCEL z: %dmg\n", Sensor_Accel.x, Sensor_Accel.y, Sensor_Accel.z);
//		UART_Driver_SendString(CMD_line_handle,msg);
//
//		CMD_process_state = 0;
//	    return;
//	}
//	default:
//		break;
//	}
//	return;
//}
//static void CMD_GET_SENSOR_LSM6DSOX(EmbeddedCli *cli, char *args, void *context){
//	char msg[100];
//	switch (CMD_process_state)
//	{
//	case 0:
//	{
//		int argc = embeddedCliGetTokenCount(args);
//		if(argc != 0){
//			embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//			return;
//		}
//		Sensor_Read_Value(SENSOR_READ_LSM6DSOX);
//		CMD_process_state = 1;
//		return;
//	}
//
//
//	case 1:
//	{
//		i2c_result_t return_value = Is_Sensor_Read_Complete(&Sensor_LSM6DSOX_rb);
//
//		if (return_value == I2C_IS_RUNNING)
//		{
//			return;
//		}
//		else if (return_value != I2C_OK)
//		{
//			CMD_process_state = 0;
//			return;
//		}
//
//		sprintf(msg, "> GYRO x: %dmpds; GYRO y: %dmpds; GYRO z: %dmpds\n", Sensor_Gyro.x, Sensor_Gyro.y, Sensor_Gyro.z);
//		UART_Driver_SendString(CMD_line_handle,msg);
//		sprintf(msg, "> ACCEL x: %dmg; ACCEL y: %dmg; ACCEL z: %dmg\n", Sensor_Accel.x, Sensor_Accel.y, Sensor_Accel.z);
//		UART_Driver_SendString(CMD_line_handle,msg);
//		CMD_process_state = 0;
//	    return;
//	}
//	default:
//		break;
//	}
//	return;
//}
//static void CMD_GET_SENSOR_TEMP(EmbeddedCli *cli, char *args, void *context){
//
//	char msg[100];
//
//	switch (CMD_process_state)
//	{
//	case 0:
//	{
//		int argc = embeddedCliGetTokenCount(args);
//		if(argc != 0){
//			embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//			return;
//		}
//		Sensor_Read_Value(SENSOR_READ_TEMP);
//		CMD_process_state = 1;
//		embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//
//		return;
//	}
//	case 1:
//	{
//		i2c_result_t return_value = Is_Sensor_Read_Complete(&Sensor_BMP390_rb);
//
//		if (return_value == I2C_IS_RUNNING)
//		{
//			embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//			return;
//		}
//		else if (return_value != I2C_OK)
//		{
//			CMD_process_state = 0;
//			return;
//		}
//
//		char fractional_string[16] = {0};
//		double_to_string(Sensor_Temp, fractional_string, 3);
//
//		sprintf(msg, "> TEMPERATURE: %s C\n", fractional_string);
//		embeddedCliPrint(cli, msg);
//
//		CMD_process_state = 0;
//	    return;
//	}
//
//	default:
//		break;
//	}
//	return;
//}
//static void CMD_GET_SENSOR_PRESSURE(EmbeddedCli *cli, char *args, void *context){
//	char msg[100];
//
//	switch (CMD_process_state)
//	{
//	case 0:
//	{
//		int argc = embeddedCliGetTokenCount(args);
//		if(argc != 0){
//			embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//			return;
//		}
//		Sensor_Read_Value(SENSOR_READ_PRESSURE);
//		CMD_process_state = 1;
//		embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//		return;
//	}
//	case 1:
//	{
//		i2c_result_t return_value = Is_Sensor_Read_Complete(&Sensor_BMP390_rb);
//
//		if (return_value == I2C_IS_RUNNING)
//		{
//			embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//			return;
//		}
//		else if (return_value != I2C_OK)
//		{
//			CMD_process_state = 0;
//			return;
//		}
//
//		char fractional_string[16] = {0};
//		double_to_string(Sensor_Pressure, fractional_string, 3);
//
//		sprintf(msg, "\r\n> PRESSURE: %s Pa\n", fractional_string);
//		embeddedCliPrint(cli, msg);
//
//		CMD_process_state = 0;
//	    return;
//	}
//
//	default:
//		break;
//	}
//	return;
//}
//static void CMD_GET_SENSOR_ALTITUDE(EmbeddedCli *cli, char *args, void *context){
//	embeddedCliPrint(cli, "\r\n> Tobe continued");
//}
//static void CMD_GET_SENSOR_BMP390(EmbeddedCli *cli, char *args, void *context){
//	char msg[100];
//
//	switch (CMD_process_state)
//	{
//	case 0:
//	{
//		int argc = embeddedCliGetTokenCount(args);
//		if(argc != 0){
//			embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//			return;
//		}
//		Sensor_Read_Value(SENSOR_READ_BMP390);
//		CMD_process_state = 1;
//		embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//		return;
//	}
//	case 1:
//	{
//		i2c_result_t return_value = Is_Sensor_Read_Complete(&Sensor_BMP390_rb);
//
//		if (return_value == I2C_IS_RUNNING)
//		{
//			embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//			return;
//		}
//		else if (return_value != I2C_OK)
//		{
//			CMD_process_state = 0;
//			return;
//		}
//
//		char fractional_string[16] = {0};
//
//		double_to_string(Sensor_Temp, fractional_string, 3);
//		sprintf(msg, "\r\n> TEMPERATURE: %s C\n", fractional_string);
//		embeddedCliPrint(cli, msg);
//
//		double_to_string(Sensor_Pressure, fractional_string, 3);
//		sprintf(msg, "\r\n> PRESSURE: %s Pa\n", fractional_string);
//		embeddedCliPrint(cli, msg);
//
//		// double_to_string(Sensor_Altitude, fractional_string, 3);
//		// UART_Printf(CMD_line_handle, "> ALTITUDE: %s m\n", fractional_string);
//
//		CMD_process_state = 0;
//	    return;
//	}
//
//	default:
//		break;
//	}
//	return;
//}
//static void CMD_GET_SENSOR_H3LIS(EmbeddedCli *cli, char *args, void *context){
//	char msg[100];
//	switch (CMD_process_state)
//		{
//		case 0:
//		{
//			int argc = embeddedCliGetTokenCount(args);
//			if(argc != 0){
//				embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//				return;
//			}
//
//			Sensor_Read_Value(ONBOARD_SENSOR_READ_H3LIS331DL);
//			CMD_process_state = 1;
//			embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//			return;
//		}
//		case 1:
//		{
//			i2c_result_t return_value = Is_Sensor_Read_Complete(&Onboard_Sensor_H3LIS331DL_rb);
//
//			if (return_value == I2C_IS_RUNNING)
//			{
//				embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//				return;
//			}
//			else if (return_value != I2C_OK)
//			{
//				CMD_process_state = 0;
//				return;
//			}
//
//			sprintf(msg, "\r\n> ACCEL x: %dmg; ACCEL y: %dmg; ACCEL z: %dmg\n", H3LIS_Accel.x, H3LIS_Accel.y, H3LIS_Accel.z);
//			embeddedCliPrint(cli,msg);
//			CMD_process_state = 0;
//			return CMDLINE_OK;
//		}
//		default:
//			break;
//		}
//
//		return;
//}
//static void CMD_SET_SENSOR_H3LIS_FS(EmbeddedCli *cli, char *args, void *context){
//	char ms[100];
//	switch (CMD_process_state)
//		{
//		case 0:
//		{
//			int argc = embeddedCliGetTokenCount(args);
//			if(argc > 1){
//				embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//				return;
//			}
//			else if(argc <1){
//				embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_FEW_ARGS");
//				return;
//			}
//
//			int receive_argm;
//			receive_argm  = atoi(embeddedCliGetToken(args, 1));
//
//			if ((receive_argm != 100) && (receive_argm != 200) && (receive_argm != 400))
//			{
//				embeddedCliPrint(cli,"\n\r> CMDLINE_INVALID_ARGS");
//				return CMDLINE_INVALID_ARG;
//			}
//
//			switch (receive_argm)
//			{
//			case 100:
//				Sensor_Read_Value(ONBOARD_SENSOR_SET_FS_100G);
//				break;
//			case 200:
//				Sensor_Read_Value(ONBOARD_SENSOR_SET_FS_200G);
//				break;
//			case 400:
//				Sensor_Read_Value(ONBOARD_SENSOR_SET_FS_400G);
//				break;
//
//			default:
//				break;
//			}
//
//			current_h3lis_fs = receive_argm;
//			CMD_process_state = 1;
//			embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//			return;
//		}
//
//		case 1:
//		{
//			i2c_result_t return_value = Is_Sensor_Read_Complete(&Onboard_Sensor_H3LIS331DL_rb);
//
//			if (return_value == I2C_IS_RUNNING)
//			{
//				embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//				return;
//			}
//			else if (return_value != I2C_OK)
//			{
//				CMD_process_state = 0;
//				return;
//			}
//
//			embeddedCliPrint(cli, "\n\r> SET FULL SCALE FOR H3LIS331DL SUCCESSFULLY\n");
//			CMD_process_state = 0;
//			return;
//		}
//
//		default:
//			break;
//		}
//
//		return;
//}
//static void CMD_GET_SENSOR_H3LIS_FS(EmbeddedCli *cli, char *args, void *context){
//	char msg[100];
//	switch (CMD_process_state)
//		{
//		case 0:
//		{
//			int argc = embeddedCliGetTokenCount(args);
//			if(argc != 0){
//				embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
//				return;
//			}
//
//			Sensor_Read_Value(ONBOARD_SENSOR_GET_FS);
//			CMD_process_state = 1;
//			embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//			return;
//		}
//
//		case 1:
//		{
//			i2c_result_t return_value = Is_Sensor_Read_Complete(&Onboard_Sensor_H3LIS331DL_rb);
//
//			if (return_value == I2C_IS_RUNNING)
//			{
//				embeddedCliPrint(cli, "\n\r> CMD_Is_Processing");
//				return;
//			}
//			else if (return_value != I2C_OK)
//			{
//				CMD_process_state = 0;
//				return;
//			}
//
//			uint16_t fs = 0;
//
//			switch (H3LIS_Accel.full_scale)
//			{
//			case 0:
//				fs = 100;
//				break;
//			case 1:
//				fs = 200;
//				break;
//			case 3:
//				fs = 400;
//
//			default:
//				break;
//			}
//
//			sprintf(msg,"> H3LIS331DL FULL SCALE: %dG\n", fs);
//			embeddedCliPrint(cli,msg);
//			current_h3lis_fs = fs;
//			CMD_process_state = 0;
//			return;
//		}
//
//		default:
//			break;
//		}
//
//		return;
//}
//
//static void CMD_GET_SENSOR_ADS1115_BAT(EmbeddedCli *cli, char *args, void *context){
//	char msg[100];
//
//	int argc = embeddedCliGetTokenCount(args);
//	if(argc != 0){
//		embeddedCliPrint(cli, "\r\n> CMD_TOO_MANY_ARG");
//		rerturn;
//	}
//
//
//}
//static void CMD_GET_SENSOR_ADS1115_AC(EmbeddedCli *cli, char *args, void *context){
//
//}
//
//static void CMD_GET_SENSOR_ADS1115_CHAR(EmbeddedCli *cli, char *args, void *context){
//
//}



/*************************************************
 *                  End CMD List                 *
 *************************************************/
static void double_to_string(double value, char *buffer, uint8_t precision)
{
    // Handle negative numbers
    if (value < 0)
	{
        *buffer++ = '-';
        value = -value;
    }

    // Extract the integer part
    uint32_t integer_part  = (uint32_t)value;
    double fractional_part = value - integer_part;

    // Convert integer part to string
    sprintf(buffer, "%ld", integer_part);
    while (*buffer) buffer++; // Move pointer to the end of the integer part

    // Add decimal point
    if (precision > 0)
	{
        *buffer++ = '.';

        // Extract and convert the fractional part
        for (uint8_t i = 0; i < precision; i++)
		{
            fractional_part *= 10;
            uint8_t digit = (uint8_t)fractional_part;
            *buffer++ = '0' + digit;
            fractional_part -= digit;
        }
    }

    // Null-terminate the string
    *buffer = '\0';
}

/*************************************************
 *                Getter - Helper                *
 *************************************************/
const CliCommandBinding *getCliStaticBindings(void) {
    return cliStaticBindings_internal;
}

uint16_t getCliStaticBindingCount(void) {
    return sizeof(cliStaticBindings_internal) / sizeof(cliStaticBindings_internal[0]);
}
