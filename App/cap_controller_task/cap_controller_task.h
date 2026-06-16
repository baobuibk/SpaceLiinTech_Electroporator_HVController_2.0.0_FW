/*
 * cap_controller_task.h
 *
 *  Created on: Apr 27, 2026
 *      Author: PV
 */

#ifndef CAP_CONTROLLER_TASK_CAP_CONTROLLER_TASK_H_
#define CAP_CONTROLLER_TASK_CAP_CONTROLLER_TASK_H_

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include <string.h>

#include "board.h"
#include "app.h"
#include "pid.h"
#include "pwm.h"

//#include "cap_calib_task.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define CAP_300V   0
#define CAP_50V    1


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
typedef enum
{
    CAP_IS_IDLE,
    CAP_IS_CHARGING,
    CAP_IS_FINISH_CHARGING,
    CAP_IS_DISCHARGING,
    CAP_IS_FINISH_DISCHARGING,
} CAP_State_t;

typedef enum
{
    CAP_SOFT_START_STATE,
    CAP_CONTROL_CHARGE_STATE,
    CAP_SET_FREE_CHARGE_STATE,
    CAP_IS_FREE_CHARGE_STATE,

} Cap_Charge_State_t;

typedef struct
{
    uint16_t Volt_Value;
    uint16_t ADC_Value;
    uint8_t  Duty_Max;
} Charge_Range_t;



typedef struct {

	uint16_t 	max_cap_volt;
    uint16_t    raw_ADC_Value;
    uint16_t    set_charge_voltage_ADC;

    PID_TypeDef charge_PID;
    PWM_TypeDef charge_PWM;
    uint8_t     charge_PWM_duty;

    Cap_Charge_State_t  charge_state;
    Charge_Range_t* p_range;
    uint16_t    soft_start_count;
    uint8_t     range_index;
    uint8_t     range_max;
    uint8_t     range_set;

    float		calib_coefficient;

    bool		is_notified_on;

} Cap_Control_t;

typedef enum
{
	CAP_PRF_HV = 0,
	CAP_PRF_LV,

}Cap_Profile_t;



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */




void Cap_Controller_Init(void);

/* :::::::::: Cap Controller Charge Task ::::::::::::: */
void Cap_Controller_Charge_Task(void*);

/* :::::::::: Cap Controller Ultility Task ::::::::::::: */
void Cap_Controller_Monitor_Task(void*);

/* :::::::::: Cap Controller Command :::::::: */
void Cap_Set_Volt(Cap_Profile_t prf_cap_x, uint16_t set_voltage);
void Cap_Set_Volt_All(uint16_t HV_set_voltage, uint16_t LV_set_voltage);
void Cap_Set_Charge(Cap_Profile_t prf_cap_x, bool charge_state, bool is_notified);
void Cap_Set_Charge_All(bool HV_charge_state, bool LV_charge_state, bool HV_is_notified, bool LV_is_notified );
void Cap_Set_Discharge(Cap_Profile_t prf_cap_x, bool discharge_state);
void Cap_Set_Discharge_All(bool HV_discharge_state, bool LV_discharge_state);

uint16_t Cap_Get_Set_Volt(Cap_Profile_t prf_cap_x);
bool     Cap_Get_is_Charge(Cap_Profile_t prf_cap_x);
bool     Cap_Get_is_Discharge(Cap_Profile_t prf_cap_x);
bool     Cap_Get_is_Notified(Cap_Profile_t prf_cap_x);

uint16_t Cap_Measure_Volt(Cap_Profile_t prf_cap_x);

/* :::::::::: Cap Controller Driver Interupt Handler ::::::::::::: */
void Cap_Controller_ADC_IRQHandler(void);
void Cap_Controller_OVV_300V_flag(void);
void Cap_Controller_OVV_50V_flag(void);


#endif /* CAP_CONTROLLER_TASK_CAP_CONTROLLER_TASK_H_ */
