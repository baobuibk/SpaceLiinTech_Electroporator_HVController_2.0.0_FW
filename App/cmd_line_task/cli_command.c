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
    { NULL, "HELP",         "format: help",                                         false, NULL, CMD_Help },
    { NULL, "RESET",        "Reset MCU: reset",                                     false, NULL, CMD_Reset },             // Giữ từ ví dụ ban đầu
    { NULL, "CLR",          "Clears the console",                                   false, NULL, CMD_ClearCLI },          // Giữ từ ví dụ ban đầu


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

	if (Cap_Get_is_Charge(CAP_PRF_HV) == true)
	{
		embeddedCliPrint(cli, "\n\r> HV CAP IS CHARGING, PLEASE DISABLE CHARGING BEFORE SET NEW VOLT");
		is_return = 1;
	}

	if (Cap_Get_is_Charge(CAP_PRF_LV) == true)
	{
		embeddedCliPrint(cli, "\n\r> LV CAP IS CHARGING, PLEASE DISABLE CHARGING BEFORE SET NEW VOLT");
		is_return = 1;
	}

	if (is_return == 1)
	{
		return ;
	}

	Cap_Set_Volt_All(receive_argm[0], receive_argm[1]);
	embeddedCliPrint(cli,"\n\r> CMDLINE_OK");
	return;

}
static void	CMD_SET_CAP_VOLT_HV (EmbeddedCli *cli, char *args, void *context){

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

	if (Cap_Get_is_Charge(CAP_PRF_HV) == true)
	{
		embeddedCliPrint(cli, "\n\r> HV CAP IS CHARGING, PLEASE DISABLE CHARGING BEFORE SET NEW VOLT");
		return ;
	}

	Cap_Set_Volt(CAP_PRF_HV, receive_argm);
	embeddedCliPrint(cli,"\n\r> CMDLINE_OK");

	return;

}
static void CMD_SET_CAP_VOLT_LV (EmbeddedCli *cli, char *args, void *context){
//	if (g_is_calib_running == true)
//	{
//		return ;
//	}
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

	if (Cap_Get_is_Charge(CAP_PRF_LV) == true)
	{
		embeddedCliPrint(cli,"\n\r> LV CAP IS CHARGING, PLEASE DISABLE CHARGING BEFORE SET NEW VOLT");
		return ;
	}

	Cap_Set_Volt(CAP_PRF_LV, receive_argm);
	embeddedCliPrint(cli,"\n\r> CMDLINE_OK");
	return;

}
static void CMD_SET_CAP_CONTROL (EmbeddedCli *cli, char *args, void *context){



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
		set_volt 	 = Cap_Get_Set_Volt(CAP_PRF_HV);
		measure_volt = Cap_Measure_Volt(CAP_PRF_HV);

		if (set_volt < measure_volt)
		{
			embeddedCliPrint(cli,"\n\r> CURRENT HV VOLT IS HIGHER THAN YOUR SET VOLT, PLEASE DISCHARGE BEFORE CHARGING");
			is_return = 1;
		}
	}

	if (receive_argm[1] == 1)
	{
		set_volt 	 = Cap_Get_Set_Volt(CAP_PRF_LV);
		measure_volt = Cap_Measure_Volt(CAP_PRF_LV);

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

	Cap_Set_Charge_All(receive_argm[0], receive_argm[1],true,true);
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
	Cap_Set_Discharge_All(receive_argm[0], receive_argm[1]);
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
	hv_cap_set_voltage = Cap_Get_Set_Volt(CAP_PRF_HV);
	lv_cap_set_voltage = Cap_Get_Set_Volt(CAP_PRF_LV);

	sprintf(msg, "\n\r> HV CAP IS SET AT: %dV, LV CAP IS SET AT: %dV", hv_cap_set_voltage, lv_cap_set_voltage);
	embeddedCliPrint(cli,msg);

}
static void CMD_GET_CAP_CONTROL (EmbeddedCli *cli, char *args, void *context){

	int argc = embeddedCliGetTokenCount(args);
	if(argc != 0){
		embeddedCliPrint(cli,"\n\r> CMDLINE_TOO_MANY_ARGS");
		return;
	}
	if (Cap_Get_is_Charge(CAP_PRF_HV) == true)
	{
		embeddedCliPrint(cli, "\n\r> HV CAP IS CHARGING");
	}
	else
	{
		embeddedCliPrint(cli, "\n\r> HV CAP IS NOT CHARGING");
	}

	if (Cap_Get_is_Charge(CAP_PRF_LV) == true)
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

	if (Cap_Get_is_Discharge(CAP_PRF_HV) == true)
	{
		embeddedCliPrint(cli, "\n\r> HV CAP IS DISCHARGING");
	}
	else
	{
		embeddedCliPrint(cli, "\n\r> HV CAP IS NOT DISCHARGING");
	}

	if (Cap_Get_is_Discharge(CAP_PRF_LV) == true)
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

	hv_cap_set_voltage = Cap_Get_Set_Volt(CAP_PRF_HV);
	lv_cap_set_voltage = Cap_Get_Set_Volt(CAP_PRF_LV);

	sprintf(msg, "\n\r> HV CAP IS SET AT: %dV, LV CAP IS SET AT: %dV", hv_cap_set_voltage, lv_cap_set_voltage);
	embeddedCliPrint(cli,msg);

	if (Cap_Get_is_Charge(CAP_PRF_HV) == true)
	{
		embeddedCliPrint(cli, "\n\r> HV CAP IS CHARGING");
	}
	else
	{
		embeddedCliPrint(cli, "\n\r> HV CAP IS NOT CHARGING");
	}

	if (Cap_Get_is_Charge(CAP_PRF_LV) == true)
	{
		embeddedCliPrint(cli,"\n\r> LV CAP IS CHARGING");
	}
	else
	{
		embeddedCliPrint(cli, "\n\r> LV CAP IS NOT CHARGING");
	}

	if (Cap_Get_is_Discharge(CAP_PRF_HV) == true)
	{
		embeddedCliPrint(cli, "\n\r> HV CAP IS DISCHARGING");
	}
	else
	{
		embeddedCliPrint(cli, "\n\r> HV CAP IS NOT DISCHARGING");
	}

	if (Cap_Get_is_Discharge(CAP_PRF_LV) == true)
	{
		embeddedCliPrint(cli, "\n\r> LV CAP IS DISCHARGING");
	}
	else
	{
		embeddedCliPrint(cli, "\n\r> LV CAP IS NOT DISCHARGING");
	}

}





/*************************************************
 *                  End CMD List                 *
 *************************************************/


/*************************************************
 *                Getter - Helper                *
 *************************************************/
const CliCommandBinding *getCliStaticBindings(void) {
    return cliStaticBindings_internal;
}

uint16_t getCliStaticBindingCount(void) {
    return sizeof(cliStaticBindings_internal) / sizeof(cliStaticBindings_internal[0]);
}
