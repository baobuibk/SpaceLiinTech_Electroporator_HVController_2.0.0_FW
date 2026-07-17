/*
 * DB_charge_task.h
 *
 *  Created on: Jun 12, 2026
 *      Author: PV
 */

#ifndef DATABASE_DB_CAP_CONTROLLER_H_
#define DATABASE_DB_CAP_CONTROLLER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "cap_controller_task.h"


#define DB_OFFSET 7U

typedef enum {
    /* ----- 300V (HV) ----- */
    DB_ID_CAP_HV_STATE = 0,
    DB_ID_CAP_HV_VOLT_SET,
    DB_ID_CAP_HV_VOLT_RAW,
    DB_ID_CAP_HV_CMD_CHARGE,
    DB_ID_CAP_HV_CMD_DISCHARGE,
	DB_ID_CAP_HV_OVV_FLAG,
    DB_ID_CAP_HV_CALIB_RUNNING,

    /* ----- 50V (LV) ----- */
    DB_ID_CAP_LV_STATE ,
    DB_ID_CAP_LV_VOLT_SET,
    DB_ID_CAP_LV_VOLT_RAW,
    DB_ID_CAP_LV_CMD_CHARGE,
    DB_ID_CAP_LV_CMD_DISCHARGE,
	DB_ID_CAP_LV_OVV_FLAG,
    DB_ID_CAP_LV_CALIB_RUNNING,

	/* ----- MODE ----- */
	DB_ID_CAP_IMPEDANCE_MEASURE_MODE,
	DB_IF_CAP_AUTO_ACCEL_PULSING_MODE,

    DB_ID_CAP_MAX
} db_cap_id_t;

typedef struct {
    // HV 300V PROFILE
    CAP_State_t hv_state;
    uint16_t    hv_volt_raw;
    uint16_t    hv_volt_set;
    bool        hv_cmd_charge;
    bool        hv_cmd_discharge;
    bool 		hv_OVV_flag;
    bool        hv_calib_running;

    // LV 50V PROFILE
    CAP_State_t lv_state;
    uint16_t    lv_volt_raw;
    uint16_t    lv_volt_set;
    bool        lv_cmd_charge;
    bool        lv_cmd_discharge;
    bool 		lv_OVV_flag;
    bool        lv_calib_running;

    // MODE
    bool   		is_impedance_measure_on;
    bool 		is_auto_accel_pulsing_on;

} db_cap_data_t;


void db_cap_init(void);
bool db_cap_read(db_cap_id_t id, void* out_data);
bool db_cap_write(db_cap_id_t id, const void* in_data);

#endif /* DATABASE_DB_CAP_CONTROLLER_H_ */
