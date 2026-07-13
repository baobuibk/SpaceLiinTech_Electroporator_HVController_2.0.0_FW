/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define MY_CORE_H_
#define MY_GPIO_H_
#define MY_ADC_H_
#include "stm32f4_header.h"

#include "pid.h"
#include "pwm.h"
#include "crc.h"
#include "fsp.h"

//#include "fsp_frame.h"
#include "cap_controller_task.h"
#include "db_cap_controller.h"

#include "board.h"
#include "app.h"


#define CAP_300V_CHARNGE_RANGE_MAX 9
Charge_Range_t Cap_300V_charge_range[CAP_300V_CHARNGE_RANGE_MAX] =
{
    {
        .Volt_Value = 20,
        .Duty_Max = 10,
    },
    {
        .Volt_Value = 50,
        .Duty_Max = 20,
    },
    {
        .Volt_Value = 75,
        .Duty_Max = 25,
    },
    {
        .Volt_Value = 100,
        .Duty_Max = 35,
    },
    {
        .Volt_Value = 150,
        .Duty_Max = 40,
    },
    {
        .Volt_Value = 200,
        .Duty_Max = 45,
    },
    {
        .Volt_Value = 250,
        .Duty_Max = 45,
    },
    {
        .Volt_Value = 275,
        .Duty_Max = 45,
    },
    {
        .Volt_Value = 300,
        .Duty_Max = 45,
    }
};

#define CAP_50V_CHARNGE_RANGE_MAX 5
Charge_Range_t Cap_50V_charge_range[CAP_50V_CHARNGE_RANGE_MAX] =
{
    {
        .Volt_Value = 10,
        .Duty_Max = 10,
    },
    {
        .Volt_Value = 20,
        .Duty_Max = 15,
    },

    {
        .Volt_Value = 30,
        .Duty_Max = 20,
    },
    {
        .Volt_Value = 40,
        .Duty_Max = 20,
    },
    {
        .Volt_Value = 50,
        .Duty_Max = 20,
    }
};


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static Cap_Control_t s_Cap_300V = {

		.max_cap_volt = 300,

		.raw_ADC_Value = 0,
		.set_charge_voltage_ADC = 0,

		.charge_PWM_duty = 0,

		.charge_state = CAP_SOFT_START_STATE,
		.p_range = Cap_300V_charge_range,
		.range_max = CAP_300V_CHARNGE_RANGE_MAX,
		.range_set = 0,
		.range_index = 0,
		.soft_start_count = 0,

		.calib_coefficient = 0.08297207031f,

		.is_notified_on = false,

};
static Cap_Control_t s_Cap_50V= {

		.max_cap_volt = 50,

		.raw_ADC_Value = 0,
		.set_charge_voltage_ADC = 0,

		.charge_PWM_duty = 0,

		.charge_state = CAP_SOFT_START_STATE,
		.p_range = Cap_50V_charge_range,
		.range_max = CAP_50V_CHARNGE_RANGE_MAX,
		.range_set = 0,
		.range_index = 0,
		.soft_start_count = 0,

		.calib_coefficient = 0.01366007326f,

		.is_notified_on = false,

};

volatile static Cap_Control_t* Cap_array[2] =
{
    &s_Cap_300V,
    &s_Cap_50V,
};
volatile static uint8_t  Cap_array_index = 0;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static void Cap_ADC_Init(uint32_t ADC_Sampling_Time);
static void Cap_PWM_Init(uint32_t freq_300V, uint32_t freq_50V);
static void Cap_Discharge_Init(void);


static inline void FlyBack_PID_Compute(volatile Cap_Control_t* p_cap_x);
static inline void Flyback_Set_Freq(Cap_Control_t* p_cap_x, uint32_t _Freq);
static inline void Flyback_Set_Duty(Cap_Control_t* p_cap_x, uint32_t _Duty);

static void Cap_Controller_Charge_Monitor_300V(void);
static void Cap_Controller_Discharge_Monitor_300V(void);
static void Cap_Controller_Charge_Monitor_50V(void);
static void Cap_Controller_Discharge_Monitor_50V(void);

static uint16_t Cap_Calib_Calculate(Cap_Control_t* p_cap_x, uint16_t raw_voltage);
static uint16_t Cap_ADC_to_Volt(Cap_Control_t* p_cap_x, uint16_t ADC_value);
static void Cap_Set_Volt_Internal(Cap_Control_t* p_cap_x, uint16_t set_voltage);

static void fsp_print(uint8_t packet_length);


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: Cap Controller Init :::::::: */
void Cap_Controller_Init(void)
{
		s_Cap_300V.charge_PID = (PID_TypeDef)
	    {
	        .PID_Mode       = _PID_MODE_AUTOMATIC,
	        .PON_Type       = _PID_P_ON_E,
	        .PID_Direction  = _PID_CD_DIRECT,
	        .Kp             = 0.04 + 0.26,
	        .Ki             = 0.5,
	        .Kd             = 0.0,
	        .MyInput        = &s_Cap_300V.raw_ADC_Value,
	        .MyOutput       = &s_Cap_300V.charge_PWM_duty,
	        .MySetpoint     = &s_Cap_300V.set_charge_voltage_ADC,
	        .Output_Min     = 0,
	        .Output_Max     = 25,
	    };

	    s_Cap_50V.charge_PID = (PID_TypeDef)
	    {
	        .PID_Mode 		= 	_PID_MODE_AUTOMATIC,
	        .PON_Type 		= 	_PID_P_ON_E,
	        .PID_Direction 	=	_PID_CD_DIRECT,
	        .Kp				= 	0.04 + 0.26,
	        .Ki				= 	0.5,
	        .Kd 			=	0.0,
	        .MyInput		=	&s_Cap_50V.raw_ADC_Value,
	        .MyOutput		= 	&s_Cap_50V.charge_PWM_duty,
	        .MySetpoint		=	&s_Cap_50V.set_charge_voltage_ADC,
	        .Output_Min		= 	0,
	        .Output_Max		=	10,
	    };

	    s_Cap_300V.charge_PWM = (PWM_TypeDef)
	    {
	        .TIMx       =   FLYBACK_SW1_HANDLE,
	        .Channel    =   FLYBACK_SW1_CHANNEL,
	        .Prescaler  =   0,
	        .Mode       =   LL_TIM_OCMODE_PWM1,
	        .Polarity   =   LL_TIM_OCPOLARITY_HIGH,
	        .Duty       =   0,
	        .Freq       =   0,
	    };

	    s_Cap_50V.charge_PWM = (PWM_TypeDef)
	    {
	        .TIMx       =   FLYBACK_SW2_HANDLE,
	        .Channel    =   FLYBACK_SW2_CHANNEL,
	        .Prescaler  =   0,
	        .Mode       =   LL_TIM_OCMODE_PWM1,
	        .Polarity   =   LL_TIM_OCPOLARITY_HIGH,
	        .Duty       =   0,
	        .Freq       =   0,
	    };

	db_cap_init();

    uint32_t ADC_Sampling_Time = LL_ADC_SAMPLINGTIME_480CYCLES;
    Cap_ADC_Init(ADC_Sampling_Time);

    uint32_t PWM_300V_Freq = 70000, PWM_50V_Freq = 70000;
    Cap_PWM_Init(PWM_300V_Freq, PWM_50V_Freq);

    Cap_Discharge_Init();


    for (uint8_t Idx = 0; Idx < s_Cap_300V.range_max; Idx++)
    {
        s_Cap_300V.p_range[Idx].ADC_Value = Cap_Calib_Calculate(&s_Cap_300V, s_Cap_300V.p_range[Idx].Volt_Value);
    }

    for (uint8_t Idx = 0; Idx < s_Cap_50V.range_max; Idx++)
    {
        s_Cap_50V.p_range[Idx].ADC_Value = Cap_Calib_Calculate(&s_Cap_50V, s_Cap_50V.p_range[Idx].Volt_Value);
    }
}


/* :::::::::: Cap Controller Charge Task :::::::: */
void Cap_Controller_Charge_Task(void*)
{
	//---PROFILE 300V-----
	bool hv_cmd_charge = false;
	bool hv_cmd_discharge = false;
	bool hv_OVV_flag = false;
	uint16_t hv_set_volt = 0;

	db_cap_read(DB_ID_CAP_HV_CMD_CHARGE, &hv_cmd_charge);
	db_cap_read(DB_ID_CAP_HV_CMD_DISCHARGE, &hv_cmd_discharge);
	db_cap_read(DB_ID_CAP_HV_OVV_FLAG, &hv_OVV_flag);
	db_cap_read(DB_ID_CAP_HV_VOLT_SET, &hv_set_volt);

	if (hv_cmd_charge == true){
		if(s_Cap_300V.raw_ADC_Value >= (s_Cap_300V.set_charge_voltage_ADC*0.99)){
			if(s_Cap_300V.charge_state == CAP_SET_FREE_CHARGE_STATE){
				if(s_Cap_300V.is_notified_on == true){
					char msg[128];
					sprintf(msg,"HV CAP FINISHED CHARGING TO %dV\n\r", hv_set_volt);
					UART_Driver_SendString(CMD_line_handle,msg);

					s_Cap_300V.is_notified_on = false;
				}
			}
        CAP_State_t hv_state = CAP_IS_FINISH_CHARGING;
        db_cap_write(DB_ID_CAP_HV_STATE, &hv_state);
		}
	}
	else if(hv_cmd_charge == false){
		s_Cap_300V.charge_PWM_duty = 0;
		Flyback_Set_Duty(&s_Cap_300V, 0);
	}

	if(hv_cmd_discharge == true){
		LL_GPIO_SetOutputPin(DISCHARGE_300V_PORT,DISCHARGE_300V_PIN);
	}
	else if(hv_cmd_discharge == false){
		LL_GPIO_ResetOutputPin(DISCHARGE_300V_PORT, DISCHARGE_300V_PIN);
	}

	if(hv_OVV_flag == true){

	}
	else if(hv_OVV_flag == false){

	}

	//---PROFILE 50V-----

	bool lv_cmd_charge = false;
	bool lv_cmd_discharge = false;
	bool lv_OVV_flag = false;
	uint16_t lv_set_volt = 0;

	db_cap_read(DB_ID_CAP_LV_CMD_CHARGE, &lv_cmd_charge);
	db_cap_read(DB_ID_CAP_LV_CMD_DISCHARGE, &lv_cmd_discharge);
	db_cap_read(DB_ID_CAP_LV_OVV_FLAG, &lv_OVV_flag);
	db_cap_read(DB_ID_CAP_LV_VOLT_SET, &lv_set_volt);


	if (lv_cmd_charge == true){
		if(s_Cap_50V.raw_ADC_Value >= (s_Cap_50V.set_charge_voltage_ADC*0.99)){
			if(s_Cap_50V.charge_state == CAP_SET_FREE_CHARGE_STATE){
				if(s_Cap_50V.is_notified_on == true){
					char msg[128];
					sprintf(msg,"LV CAP FINISHED CHARGING TO %dV\n\r", lv_set_volt);
					UART_Driver_SendString(CMD_line_handle,msg);

					s_Cap_50V.is_notified_on = false;
				}

			}
         CAP_State_t lv_state = CAP_IS_FINISH_CHARGING;
         db_cap_write(DB_ID_CAP_LV_STATE, &lv_state);
		}
	}
	else if(lv_cmd_charge == false){
		s_Cap_50V.charge_PWM_duty = 0;
		Flyback_Set_Duty(&s_Cap_50V, 0);
	}

	if(lv_cmd_discharge == true){
		LL_GPIO_SetOutputPin(DISCHARGE_50V_PORT,DISCHARGE_50V_PIN);
	}
	else if(lv_cmd_discharge == false){
		LL_GPIO_ResetOutputPin(DISCHARGE_50V_PORT, DISCHARGE_50V_PIN);
	}

	if(lv_OVV_flag == true){

	}
	else if(lv_OVV_flag == false){

	}

}

void Cap_Controller_Monitor_Task(void*)
{
    Cap_Controller_Charge_Monitor_300V();
    Cap_Controller_Discharge_Monitor_300V();

    Cap_Controller_Charge_Monitor_50V();
    Cap_Controller_Discharge_Monitor_50V();
}


/* :::::::::: Cap Controller Command :::::::: */
void Cap_Set_Volt(Cap_Profile_t prf_cap_x, uint16_t set_voltage)
{
	if(prf_cap_x == CAP_PRF_HV) db_cap_write(DB_ID_CAP_HV_VOLT_SET, &set_voltage);
	else if(prf_cap_x == CAP_PRF_LV) db_cap_write(DB_ID_CAP_LV_VOLT_SET, &set_voltage);
}

void Cap_Set_Volt_All(uint16_t HV_set_voltage, uint16_t LV_set_voltage)
{
    db_cap_write(DB_ID_CAP_HV_VOLT_SET, &HV_set_voltage);

    db_cap_write(DB_ID_CAP_LV_VOLT_SET, &LV_set_voltage);
}

void Cap_Set_Charge(Cap_Profile_t prf_cap_x, bool charge_state, bool is_notified)
{
	bool cap_cmd_discharge = false;
	CAP_State_t cap_state;

	if(prf_cap_x == CAP_PRF_HV){

		if(charge_state == true){
				cap_cmd_discharge = false;
				db_cap_write(DB_ID_CAP_HV_CMD_DISCHARGE, &cap_cmd_discharge);

				cap_state = CAP_IS_CHARGING;
				db_cap_write(DB_ID_CAP_HV_STATE, &cap_state);
		}
		else{
				cap_state = CAP_IS_IDLE;
				db_cap_write(DB_ID_CAP_HV_STATE, &cap_state);
		}
		db_cap_write(DB_ID_CAP_HV_CMD_DISCHARGE, &charge_state);

	    s_Cap_300V.soft_start_count = 0;

	    s_Cap_300V.charge_state = CAP_SOFT_START_STATE;

	    s_Cap_300V.is_notified_on = is_notified;

	}

	else if(prf_cap_x == CAP_PRF_LV){

		if(charge_state == true){
				cap_cmd_discharge = false;
				db_cap_write(DB_ID_CAP_LV_CMD_CHARGE, &cap_cmd_discharge);

				cap_state = CAP_IS_CHARGING;
				db_cap_write(DB_ID_CAP_LV_STATE, &cap_state);
		}
		else{
				cap_state = CAP_IS_IDLE;
				db_cap_write(DB_ID_CAP_LV_STATE, &cap_state);
		}
		db_cap_write(DB_ID_CAP_LV_CMD_DISCHARGE, &charge_state);

		s_Cap_50V.soft_start_count = 0;

		s_Cap_50V.charge_state = CAP_SOFT_START_STATE;

		s_Cap_50V.is_notified_on = is_notified;
	}
}

void Cap_Set_Charge_All(bool HV_charge_state, bool LV_charge_state,bool HV_is_notified, bool LV_is_notified )
{
	//------------ FOR 300V -------------------
		bool hv_cmd_discharge = false;
		CAP_State_t hv_state;


		if(HV_charge_state == true){
			hv_cmd_discharge = false;
			db_cap_write(DB_ID_CAP_HV_CMD_DISCHARGE, &hv_cmd_discharge);

			hv_state = CAP_IS_CHARGING;
			db_cap_write(DB_ID_CAP_HV_STATE, &hv_state);
		}
		else{
			hv_state = CAP_IS_IDLE;
			db_cap_write(DB_ID_CAP_HV_STATE, &hv_state);
		}

	//------------ FOR 50V -------------------

		bool lv_cmd_discharge = false;
		CAP_State_t lv_state;

		if(LV_charge_state == true){
			lv_cmd_discharge = false;
			db_cap_write(DB_ID_CAP_LV_CMD_DISCHARGE, &lv_cmd_discharge);

			lv_state = CAP_IS_CHARGING;
			db_cap_write(DB_ID_CAP_LV_STATE, &lv_state);
		}
		else{
			lv_state = CAP_IS_IDLE;
			db_cap_write(DB_ID_CAP_LV_STATE, &lv_state);
		}

		db_cap_write(DB_ID_CAP_HV_CMD_CHARGE, &HV_charge_state);
		db_cap_write(DB_ID_CAP_LV_CMD_CHARGE, &LV_charge_state);

	    s_Cap_300V.soft_start_count = 0;
	    s_Cap_50V.soft_start_count = 0;

	    s_Cap_300V.charge_state = CAP_SOFT_START_STATE;
	    s_Cap_50V.charge_state = CAP_SOFT_START_STATE;

	    s_Cap_300V.is_notified_on   = HV_is_notified;
	    s_Cap_50V.is_notified_on    = LV_is_notified;
}

void Cap_Set_Discharge(Cap_Profile_t prf_cap_x, bool discharge_state)
{
	bool cap_cmd_charge = false;
	CAP_State_t cap_state;

	if(prf_cap_x == CAP_PRF_HV){

		if(discharge_state == true){
				cap_cmd_charge = false;
				db_cap_write(DB_ID_CAP_HV_CMD_CHARGE, &cap_cmd_charge);

				cap_state = CAP_IS_DISCHARGING;
				db_cap_write(DB_ID_CAP_HV_STATE, &cap_state);

				PID_SetOutputLimits(&s_Cap_300V.charge_PID, 0, 5);
		}
		else{
				cap_state = CAP_IS_IDLE;
				db_cap_write(DB_ID_CAP_HV_STATE, &cap_state);
		}
		db_cap_write(DB_ID_CAP_HV_CMD_DISCHARGE, &discharge_state);
	}

	else if(prf_cap_x == CAP_PRF_LV){

		if(discharge_state == true){
				cap_cmd_charge = false;
				db_cap_write(DB_ID_CAP_LV_CMD_CHARGE, &cap_cmd_charge);

				cap_state = CAP_IS_DISCHARGING;
				db_cap_write(DB_ID_CAP_LV_STATE, &cap_state);

				PID_SetOutputLimits(&s_Cap_50V.charge_PID, 0, 5);
		}
		else{
				cap_state = CAP_IS_IDLE;
				db_cap_write(DB_ID_CAP_LV_STATE, &cap_state);
		}
		db_cap_write(DB_ID_CAP_LV_CMD_DISCHARGE, &discharge_state);
	}
}

void Cap_Set_Discharge_All(bool HV_discharge_state, bool LV_discharge_state)
{
	//------------ FOR 300V -------------------
	bool hv_cmd_charge = false;
	CAP_State_t hv_state;

	if(HV_discharge_state == true)
	{
		hv_cmd_charge = false;
		db_cap_write(DB_ID_CAP_HV_CMD_CHARGE, &hv_cmd_charge);

		hv_state = CAP_IS_DISCHARGING;
		db_cap_write(DB_ID_CAP_HV_STATE, &hv_state);

		PID_SetOutputLimits(&s_Cap_300V.charge_PID, 0, 5);
	}
	else
	{
		hv_state = CAP_IS_IDLE;
		db_cap_write(DB_ID_CAP_HV_STATE, &hv_state);
	}

	db_cap_write(DB_ID_CAP_HV_CMD_DISCHARGE, &HV_discharge_state);

	//------------ FOR 50V -------------------
	bool lv_cmd_charge = false;
	CAP_State_t lv_state;

	if(LV_discharge_state == true){
		lv_cmd_charge = false;
		db_cap_write(DB_ID_CAP_LV_CMD_CHARGE, &lv_cmd_charge);

		lv_state = CAP_IS_DISCHARGING;
		db_cap_write(DB_ID_CAP_LV_STATE, &lv_state);

		PID_SetOutputLimits(&s_Cap_50V.charge_PID, 0, 5);
	}
	else{
		lv_state = CAP_IS_IDLE;
		db_cap_write(DB_ID_CAP_LV_STATE, &lv_state);
	}
	db_cap_write(DB_ID_CAP_LV_CMD_DISCHARGE, &LV_discharge_state);
}

uint16_t Cap_Get_Set_Volt(Cap_Profile_t prf_cap_x)
{
	uint16_t cap_set_volt = 0;

	if(prf_cap_x == CAP_PRF_HV) db_cap_read(DB_ID_CAP_HV_VOLT_SET, &cap_set_volt);

	else if(prf_cap_x == CAP_PRF_LV) db_cap_read(DB_ID_CAP_LV_VOLT_SET, &cap_set_volt);

    return cap_set_volt;
}

bool Cap_Get_is_Charge(Cap_Profile_t prf_cap_x)
{
	bool cap_cmd_charge = false;

	if(prf_cap_x == CAP_PRF_HV) db_cap_read(DB_ID_CAP_HV_CMD_CHARGE, &cap_cmd_charge);

	else if(prf_cap_x == CAP_PRF_LV) db_cap_read(DB_ID_CAP_LV_CMD_CHARGE, &cap_cmd_charge);

    return cap_cmd_charge;
}

bool Cap_Get_is_Discharge(Cap_Profile_t prf_cap_x)
{
	bool cap_cmd_discharge = false;
	if(prf_cap_x == CAP_PRF_HV) db_cap_read(DB_ID_CAP_HV_CMD_DISCHARGE, &cap_cmd_discharge);

	else if(prf_cap_x == CAP_PRF_LV) db_cap_read(DB_ID_CAP_LV_CMD_DISCHARGE, &cap_cmd_discharge);

    return cap_cmd_discharge;
}

uint16_t Cap_Measure_Volt(Cap_Profile_t prf_cap_x)
{
    uint16_t ADC_value_temp = 0;
    uint16_t Raw_value_temp = 0;
    Cap_Control_t* p_cap_x;

    if (prf_cap_x == CAP_PRF_HV)
    {
    	p_cap_x = &s_Cap_300V;
        ADC_value_temp = s_Cap_300V.raw_ADC_Value;
        Raw_value_temp = Cap_ADC_to_Volt(&s_Cap_300V, ADC_value_temp);

        db_cap_write(DB_ID_CAP_HV_VOLT_RAW, &Raw_value_temp);
    }
    else if (prf_cap_x == CAP_PRF_LV)
    {
    	p_cap_x = &s_Cap_50V;
        ADC_value_temp = s_Cap_50V.raw_ADC_Value;
        Raw_value_temp = Cap_ADC_to_Volt(&s_Cap_50V, ADC_value_temp);

        db_cap_write(DB_ID_CAP_LV_VOLT_RAW, &Raw_value_temp);
    }

    return Cap_ADC_to_Volt(p_cap_x, ADC_value_temp);
}


/* :::::::::: Cap Controller Interupt Handler ::::::::::::: */
void Cap_Controller_ADC_IRQHandler(void)
{
    // Overrun handle
    if(LL_ADC_IsActiveFlag_OVR(ADC_FEEDBACK_HANDLE) == true)
    {
        LL_ADC_ClearFlag_OVR(ADC_FEEDBACK_HANDLE);

        // Reset index để đồng bộ với rank 1
        Cap_array_index = 0;

        // Dừng để reset sequencer
        LL_ADC_Disable(ADC_FEEDBACK_HANDLE);
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        LL_ADC_Enable(ADC_FEEDBACK_HANDLE);

        LL_ADC_REG_StartConversionSWStart(ADC_FEEDBACK_HANDLE);
    }

    // End of Conversion handle
    if(LL_ADC_IsActiveFlag_EOCS(ADC_FEEDBACK_HANDLE) == true)
    {
        LL_ADC_ClearFlag_EOCS(ADC_FEEDBACK_HANDLE);

        Cap_array[Cap_array_index]->raw_ADC_Value = LL_ADC_REG_ReadConversionData12(ADC_FEEDBACK_HANDLE);

        FlyBack_PID_Compute(Cap_array[Cap_array_index]);

        Cap_array_index ^= 1;
    }
}


void HV_OVV_Flag_Handle(void){
    bool hv_OVV = true;
    db_cap_write(DB_ID_CAP_HV_OVV_FLAG, &hv_OVV);

	s_Cap_300V.charge_PWM_duty = 0;
	Flyback_Set_Duty(&s_Cap_300V, 0);

	Cap_Set_Discharge(CAP_PRF_HV, true);

	ps_FSP_TX -> CMD = FSP_CMD_GET_OVV_FLAG;
	ps_FSP_TX -> Payload.get_ovv_flag.HV_OVV_flag = hv_OVV;
	ps_FSP_TX -> Payload.get_ovv_flag.LV_OVV_flag = false;

	fsp_print(3);
}

void LV_OVV_Flag_Handle(void){
    bool lv_OVV = true;
    db_cap_write(DB_ID_CAP_LV_OVV_FLAG, &lv_OVV);

	s_Cap_50V.charge_PWM_duty = 0;
	Flyback_Set_Duty(&s_Cap_50V, 0);

	Cap_Set_Discharge(CAP_PRF_LV, true);

	ps_FSP_TX -> CMD = FSP_CMD_GET_OVV_FLAG;
	ps_FSP_TX -> Payload.get_ovv_flag.HV_OVV_flag =	false ;
	ps_FSP_TX -> Payload.get_ovv_flag.LV_OVV_flag = lv_OVV;

	fsp_print(3);
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static void Cap_ADC_Init(uint32_t ADC_Sampling_Time)
{
    // Init the value of the cap array index
    Cap_array_index = 0;

    // The total number of conversions in the regular group, which is 2
    LL_ADC_REG_SetSequencerLength(ADC_FEEDBACK_HANDLE, LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS);

    // Their order in the conversion sequence
    LL_ADC_REG_SetSequencerRanks(ADC_FEEDBACK_HANDLE, LL_ADC_REG_RANK_1, ADC_300V_CHANNEL);

    LL_ADC_REG_SetSequencerRanks(ADC_FEEDBACK_HANDLE, LL_ADC_REG_RANK_2, ADC_50V_CHANNEL);

    // Setting sampling time for each channel
    LL_ADC_SetChannelSamplingTime(ADC_FEEDBACK_HANDLE, ADC_300V_CHANNEL, ADC_Sampling_Time);

    LL_ADC_SetChannelSamplingTime(ADC_FEEDBACK_HANDLE, ADC_50V_CHANNEL, ADC_Sampling_Time);

    // Enable scan mode
    LL_ADC_SetSequencersScanMode(ADC_FEEDBACK_HANDLE, LL_ADC_SEQ_SCAN_ENABLE);

    // Enable continuos mode
    LL_ADC_REG_SetContinuousMode(ADC_FEEDBACK_HANDLE, LL_ADC_REG_CONV_CONTINUOUS);

    // Since we are using continuos mode so over run is
    // inevitable
    LL_ADC_EnableIT_OVR(ADC_FEEDBACK_HANDLE);

    // Enable end-of-conversion interupt
    LL_ADC_EnableIT_EOCS(ADC_FEEDBACK_HANDLE);

    LL_ADC_REG_SetFlagEndOfConversion(ADC_FEEDBACK_HANDLE, LL_ADC_REG_FLAG_EOC_UNITARY_CONV);

    // Enable ADC itself, since we using Continuos mode, to turn the ADC off
    // Use LL_ADC_Disable(ADC_FEEDBACK_HANDLE); to turn it off.
    LL_ADC_Enable(ADC_FEEDBACK_HANDLE);

    // Finally a signal to start the Continuos mode ADC.
    LL_ADC_REG_SetTriggerSource(ADC_FEEDBACK_HANDLE, LL_ADC_REG_TRIG_SOFTWARE);
    LL_ADC_REG_StartConversionSWStart(ADC_FEEDBACK_HANDLE);
}

static void Cap_PWM_Init(uint32_t freq_300V, uint32_t freq_50V)
{
	PID_Init(&s_Cap_300V.charge_PID);

    LL_TIM_OC_SetMode(s_Cap_300V.charge_PWM.TIMx, s_Cap_300V.charge_PWM.Channel,  LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetPolarity(s_Cap_300V.charge_PWM.TIMx, s_Cap_300V.charge_PWM.Channel, LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_OC_EnablePreload(s_Cap_300V.charge_PWM.TIMx, s_Cap_300V.charge_PWM.Channel);
    LL_TIM_CC_EnableChannel(s_Cap_300V.charge_PWM.TIMx, s_Cap_300V.charge_PWM.Channel);

    LL_TIM_SetOffStates(s_Cap_300V.charge_PWM.TIMx, LL_TIM_OSSI_ENABLE, LL_TIM_OSSR_ENABLE);

    LL_TIM_DisableAutomaticOutput(s_Cap_300V.charge_PWM.TIMx);

    LL_TIM_EnableAllOutputs(s_Cap_300V.charge_PWM.TIMx);

    LL_TIM_SetPrescaler(s_Cap_300V.charge_PWM.TIMx, 0);

    LL_TIM_EnableARRPreload(s_Cap_300V.charge_PWM.TIMx);

	Flyback_Set_Freq(&s_Cap_300V, freq_300V);
	LL_TIM_DisableIT_UPDATE(s_Cap_300V.charge_PWM.TIMx);

	// Init 50V PWM PID
	PID_Init(&s_Cap_50V.charge_PID);

	//PWM_Init(&s_Cap_50V.charge_PWM);
    LL_TIM_OC_SetMode(s_Cap_50V.charge_PWM.TIMx, s_Cap_50V.charge_PWM.Channel,  LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetPolarity(s_Cap_50V.charge_PWM.TIMx, s_Cap_50V.charge_PWM.Channel, LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_OC_EnablePreload(s_Cap_50V.charge_PWM.TIMx, s_Cap_50V.charge_PWM.Channel);
    LL_TIM_CC_EnableChannel(s_Cap_50V.charge_PWM.TIMx, s_Cap_50V.charge_PWM.Channel);

    LL_TIM_SetOffStates(s_Cap_50V.charge_PWM.TIMx, LL_TIM_OSSI_ENABLE, LL_TIM_OSSR_ENABLE);

    LL_TIM_DisableAutomaticOutput(s_Cap_50V.charge_PWM.TIMx);

    LL_TIM_EnableAllOutputs(s_Cap_50V.charge_PWM.TIMx);

    LL_TIM_SetPrescaler(s_Cap_50V.charge_PWM.TIMx, 0);

    LL_TIM_EnableARRPreload(s_Cap_50V.charge_PWM.TIMx);

	Flyback_Set_Freq(&s_Cap_50V, freq_50V);
	LL_TIM_DisableIT_UPDATE(s_Cap_50V.charge_PWM.TIMx);

	LL_GPIO_SetOutputPin(FLYBACK_SD1_PORT, FLYBACK_SD1_PIN);
	LL_GPIO_SetOutputPin(FLYBACK_SD2_PORT, FLYBACK_SD2_PIN);

    LL_GPIO_ResetOutputPin(DISCHARGE_300V_PORT, DISCHARGE_300V_PIN);
    LL_GPIO_ResetOutputPin(DISCHARGE_50V_PORT, DISCHARGE_50V_PIN);

    LL_TIM_EnableCounter(s_Cap_300V.charge_PWM.TIMx);
    LL_TIM_EnableCounter(s_Cap_50V.charge_PWM.TIMx);
}

static void Cap_Discharge_Init(void)
{
    LL_GPIO_ResetOutputPin(DISCHARGE_300V_PORT, DISCHARGE_300V_PIN);
    LL_GPIO_ResetOutputPin(DISCHARGE_50V_PORT, DISCHARGE_50V_PIN);
}

static inline void FlyBack_PID_Compute(volatile Cap_Control_t* p_cap_x)
{
	bool cap_cmd_charge = false;

	if(p_cap_x == &s_Cap_50V){
		db_cap_read(DB_ID_CAP_LV_CMD_CHARGE, &cap_cmd_charge);
	}
	else if(p_cap_x == &s_Cap_300V){
		db_cap_read(DB_ID_CAP_HV_CMD_CHARGE, &cap_cmd_charge);
	}

    if (cap_cmd_charge == true)
    {
        PID_Compute(&p_cap_x->charge_PID);

        volatile PWM_TypeDef* PWMx = &p_cap_x->charge_PWM;
        uint32_t _Duty = p_cap_x->charge_PWM_duty;

        PWMx->Duty = (PWMx->Freq * (_Duty / 100.0));
        switch (PWMx->Channel)
        {
        case LL_TIM_CHANNEL_CH1:
            LL_TIM_OC_SetCompareCH1(PWMx->TIMx, PWMx->Duty);
            break;
        case LL_TIM_CHANNEL_CH2:
            LL_TIM_OC_SetCompareCH2(PWMx->TIMx, PWMx->Duty);
            break;
        case LL_TIM_CHANNEL_CH3:
            LL_TIM_OC_SetCompareCH3(PWMx->TIMx, PWMx->Duty);
            break;
        case LL_TIM_CHANNEL_CH4:
            LL_TIM_OC_SetCompareCH4(PWMx->TIMx, PWMx->Duty);
            break;

        default:
            break;
        }
    }
}

static inline void Flyback_Set_Freq(Cap_Control_t* p_cap_x, uint32_t _Freq)
{
    PWM_TypeDef* PWMx = &p_cap_x->charge_PWM;

    LL_TIM_DisableUpdateEvent(PWMx->TIMx);

    PWMx->Freq = (float)APB1_TIMER_CLK / ((float)(LL_TIM_GetPrescaler(PWMx->TIMx) + 1) * _Freq);
    LL_TIM_SetAutoReload(PWMx->TIMx, PWMx->Freq);

    LL_TIM_EnableUpdateEvent(PWMx->TIMx);
}

static inline void Flyback_Set_Duty(Cap_Control_t* p_cap_x, uint32_t _Duty)
{
    PWM_TypeDef* PWMx = &p_cap_x->charge_PWM;

    PWMx->Duty = (PWMx->Freq * (_Duty / 100.0));
    switch (PWMx->Channel)
    {
    case LL_TIM_CHANNEL_CH1:
        LL_TIM_OC_SetCompareCH1(PWMx->TIMx, PWMx->Duty);
        break;
    case LL_TIM_CHANNEL_CH2:
        LL_TIM_OC_SetCompareCH2(PWMx->TIMx, PWMx->Duty);
        break;
    case LL_TIM_CHANNEL_CH3:
        LL_TIM_OC_SetCompareCH3(PWMx->TIMx, PWMx->Duty);
        break;
    case LL_TIM_CHANNEL_CH4:
        LL_TIM_OC_SetCompareCH4(PWMx->TIMx, PWMx->Duty);
        break;

    default:
        break;
    }
}

/* :::::::::: Cap Controller Notify :::::::: */
int cnt300 = 0;
static void Cap_Controller_Charge_Monitor_300V(void)
{
	cnt300++;
	bool hv_cmd_charge = false;
	CAP_State_t hv_state;
	uint16_t hv_set_volt;

	db_cap_read(DB_ID_CAP_HV_CMD_CHARGE, &hv_cmd_charge);

	if(hv_cmd_charge == false) return;

	switch(s_Cap_300V.charge_state)
	{
	case CAP_SOFT_START_STATE:

        if (s_Cap_300V.soft_start_count == 0){
            PID_SetOutputLimits(&s_Cap_300V.charge_PID, 0, 5);
            Cap_Set_Volt_Internal(&s_Cap_300V, 5);
            s_Cap_300V.soft_start_count++;
            break;
        }

        else if (s_Cap_300V.soft_start_count < 4000){
            s_Cap_300V.soft_start_count++;
            break;
        }

        for (uint8_t Idx = 0; Idx < s_Cap_300V.range_max; Idx++)
        {
            if (s_Cap_300V.raw_ADC_Value <= s_Cap_300V.p_range[Idx].ADC_Value)
            {
                s_Cap_300V.range_index = Idx;
                break;
            }
        }

        s_Cap_300V.charge_state = CAP_CONTROL_CHARGE_STATE;
        break;

	case CAP_CONTROL_CHARGE_STATE:


		db_cap_read(DB_ID_CAP_HV_STATE, &hv_state);

		if(hv_state != CAP_IS_FINISH_CHARGING) break;

		db_cap_read(DB_ID_CAP_HV_VOLT_SET, &hv_set_volt);

		if(hv_set_volt > s_Cap_300V.p_range[s_Cap_300V.range_index].Volt_Value){
            PID_SetOutputLimits(&s_Cap_300V.charge_PID, 0, s_Cap_300V.p_range[s_Cap_300V.range_index].Duty_Max);
            Cap_Set_Volt_Internal(&s_Cap_300V, s_Cap_300V.p_range[s_Cap_300V.range_index].Volt_Value);

            hv_state = CAP_IS_CHARGING;
            db_cap_write(DB_ID_CAP_HV_STATE, &hv_state);
		}
		else{
            PID_SetOutputLimits(&s_Cap_300V.charge_PID, 0, s_Cap_300V.p_range[s_Cap_300V.range_index].Duty_Max);
            Cap_Set_Volt_Internal(&s_Cap_300V, hv_set_volt);

            hv_state = CAP_IS_CHARGING;
            db_cap_write(DB_ID_CAP_HV_STATE, &hv_state);
            s_Cap_300V.charge_state = CAP_SET_FREE_CHARGE_STATE;
            break;
		}

		s_Cap_300V.range_index++;
		break;

	case CAP_SET_FREE_CHARGE_STATE:


		db_cap_read(DB_ID_CAP_HV_STATE, &hv_state);

		if(hv_state != CAP_IS_FINISH_CHARGING) break;

	default:
		break;
	}

}

static void Cap_Controller_Discharge_Monitor_300V(void)
{

	bool hv_cmd_discharge = false;
	CAP_State_t hv_state;

	db_cap_read(DB_ID_CAP_HV_CMD_DISCHARGE, &hv_cmd_discharge);

	if(hv_cmd_discharge == false) return;

	if(s_Cap_300V.raw_ADC_Value <= 20){

        UART_Driver_SendString(CMD_line_handle,"HV CAP FINISHED RELEASING TO 0V\n\r");
        Cap_Set_Discharge(CAP_PRF_HV, false);

        hv_state = CAP_IS_FINISH_DISCHARGING;
        db_cap_write(DB_ID_CAP_HV_STATE, &hv_state);

	}

}

static void Cap_Controller_Charge_Monitor_50V(void)
{
	bool lv_cmd_charge = false;
	CAP_State_t lv_state;
	uint16_t lv_set_volt;

	db_cap_read(DB_ID_CAP_LV_CMD_CHARGE, &lv_cmd_charge);

	if(lv_cmd_charge == false) return;

	switch(s_Cap_50V.charge_state)
	{
	case CAP_SOFT_START_STATE:

        if (s_Cap_50V.soft_start_count == 0){
            PID_SetOutputLimits(&s_Cap_50V.charge_PID, 0, 5);
            Cap_Set_Volt_Internal(&s_Cap_50V, 5);
            s_Cap_50V.soft_start_count++;
            break;
        }

        else if (s_Cap_50V.soft_start_count < 4000){
            s_Cap_50V.soft_start_count++;
            break;
        }

        for (uint8_t Idx = 0; Idx < s_Cap_50V.range_max; Idx++)
        {
            if (s_Cap_50V.raw_ADC_Value <= s_Cap_50V.p_range[Idx].ADC_Value)
            {
                s_Cap_50V.range_index = Idx;
                break;
            }
        }

        s_Cap_50V.charge_state = CAP_CONTROL_CHARGE_STATE;
        break;

	case CAP_CONTROL_CHARGE_STATE:

		db_cap_read(DB_ID_CAP_LV_STATE, &lv_state);
		if(lv_state != CAP_IS_FINISH_CHARGING) break;

		db_cap_read(DB_ID_CAP_LV_VOLT_SET, &lv_set_volt);

		if(lv_set_volt > s_Cap_50V.p_range[s_Cap_50V.range_index].Volt_Value){
            PID_SetOutputLimits(&s_Cap_50V.charge_PID, 0, s_Cap_50V.p_range[s_Cap_50V.range_index].Duty_Max);
            Cap_Set_Volt_Internal(&s_Cap_50V, s_Cap_50V.p_range[s_Cap_50V.range_index].Volt_Value);

            lv_state = CAP_IS_CHARGING;
            db_cap_write(DB_ID_CAP_LV_STATE, &lv_state);
		}
		else{
            PID_SetOutputLimits(&s_Cap_50V.charge_PID, 0, s_Cap_50V.p_range[s_Cap_50V.range_index].Duty_Max);
            Cap_Set_Volt_Internal(&s_Cap_50V, lv_set_volt);

            lv_state = CAP_IS_CHARGING;
            db_cap_write(DB_ID_CAP_LV_STATE, &lv_state);

            s_Cap_50V.charge_state = CAP_SET_FREE_CHARGE_STATE;
            break;
		}

		s_Cap_50V.range_index++;
		break;

	case CAP_SET_FREE_CHARGE_STATE:

		db_cap_read(DB_ID_CAP_LV_STATE, &lv_state);
		if(lv_state != CAP_IS_FINISH_CHARGING) break;

//        float set_duty_temp = ((float)lv_set_volt * 100.0) / (120.0 + (float)lv_set_volt);
//        uint16_t cap_50V_set_duty    = (set_duty_temp * 1.112641084) + 0.5;
//
//        PID_SetOutputLimits(&s_Cap_50V.charge_PID, 0, cap_50V_set_duty);
//        s_Cap_50V.charge_state = CAP_IS_FREE_CHARGE_STATE;
        break;

	case CAP_IS_FREE_CHARGE_STATE:
		break;

	default:
		break;
	}

}

static void Cap_Controller_Discharge_Monitor_50V(void)
{
	bool lv_cmd_discharge = false;
	CAP_State_t lv_state;

	db_cap_read(DB_ID_CAP_LV_CMD_DISCHARGE, &lv_cmd_discharge);

	if(lv_cmd_discharge == false) return;

	if(s_Cap_50V.raw_ADC_Value <= 60){

        UART_Driver_SendString(CMD_line_handle,"LV CAP FINISHED RELEASING TO 0V\n\r");
        Cap_Set_Discharge(CAP_PRF_LV, false);

        lv_state = CAP_IS_FINISH_DISCHARGING;
        db_cap_write(DB_ID_CAP_LV_STATE, &lv_state);

	}
}

static uint16_t Cap_Calib_Calculate(Cap_Control_t* p_cap_x, uint16_t raw_voltage)
{
    return raw_voltage / p_cap_x->calib_coefficient;
}

static uint16_t Cap_ADC_to_Volt(Cap_Control_t* p_cap_x, uint16_t ADC_value)
{
   return (uint16_t) (ADC_value * p_cap_x->calib_coefficient);
}

static void Cap_Set_Volt_Internal(Cap_Control_t* p_cap_x, uint16_t set_voltage)
{
    p_cap_x->set_charge_voltage_ADC = Cap_Calib_Calculate(p_cap_x, set_voltage);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FSP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static void fsp_print(uint8_t packet_length)
{
	s_FSP_TX_Packet.sod 		= FSP_PKT_SOD;
	s_FSP_TX_Packet.src_adr 	= fsp_my_adr;
	s_FSP_TX_Packet.dst_adr 	= FSP_ADR_GPP;
	s_FSP_TX_Packet.length 		= packet_length;
	s_FSP_TX_Packet.type 		= FSP_PKT_TYPE_CMD_W_DATA;
	s_FSP_TX_Packet.eof 		= FSP_PKT_EOF;
	s_FSP_TX_Packet.crc16 		= crc16_CCITT(FSP_CRC16_INITIAL_VALUE, &s_FSP_TX_Packet.src_adr, s_FSP_TX_Packet.length + 4);

	uint8_t encoded_frame[25] = { 0 };
	uint8_t frame_len;
	fsp_encode(&s_FSP_TX_Packet, encoded_frame, &frame_len);

	UART_Driver_SendFSP(&GPP_UART, (char*)encoded_frame , frame_len);
}



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

