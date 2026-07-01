
#include "db_cap_controller.h"
#include <string.h>
#include "stdbool.h"


static db_cap_data_t cap_db;


typedef struct {
    void* data_ptr;
    size_t data_size;
} db_lookup_t;

static const db_lookup_t db_map[DB_ID_CAP_MAX] = {
    // Mapping  HV
    [DB_ID_CAP_HV_STATE]         = { &cap_db.hv_state,         sizeof(cap_db.hv_state) },
    [DB_ID_CAP_HV_VOLT_RAW]  	 = { &cap_db.hv_volt_raw,  	   sizeof(cap_db.hv_volt_raw) },
    [DB_ID_CAP_HV_VOLT_SET]      = { &cap_db.hv_volt_set,      sizeof(cap_db.hv_volt_set) },
    [DB_ID_CAP_HV_CMD_CHARGE]    = { &cap_db.hv_cmd_charge,    sizeof(cap_db.hv_cmd_charge) },
    [DB_ID_CAP_HV_CMD_DISCHARGE] = { &cap_db.hv_cmd_discharge, sizeof(cap_db.hv_cmd_discharge) },
	[DB_ID_CAP_HV_OVV_FLAG]		 = { &cap_db.hv_OVV_flag,      sizeof(cap_db.hv_OVV_flag)},
    [DB_ID_CAP_HV_CALIB_RUNNING] = { &cap_db.hv_calib_running, sizeof(cap_db.hv_calib_running) },

    // Mapping  LV
    [DB_ID_CAP_LV_STATE]         = { &cap_db.lv_state,         sizeof(cap_db.lv_state) },
    [DB_ID_CAP_LV_VOLT_RAW]  	 = { &cap_db.lv_volt_raw,  	   sizeof(cap_db.lv_volt_raw) },
    [DB_ID_CAP_LV_VOLT_SET]      = { &cap_db.lv_volt_set,      sizeof(cap_db.lv_volt_set) },
    [DB_ID_CAP_LV_CMD_CHARGE]    = { &cap_db.lv_cmd_charge,    sizeof(cap_db.lv_cmd_charge) },
    [DB_ID_CAP_LV_CMD_DISCHARGE] = { &cap_db.lv_cmd_discharge, sizeof(cap_db.lv_cmd_discharge) },
	[DB_ID_CAP_LV_OVV_FLAG]		 = { &cap_db.lv_OVV_flag,      sizeof(cap_db.lv_OVV_flag)},
    [DB_ID_CAP_LV_CALIB_RUNNING] = { &cap_db.lv_calib_running, sizeof(cap_db.lv_calib_running)},

	//MAPPING FLAG MODE
	[DB_ID_CAP_IMPEDANCE_MEASURE_MODE]  = { &cap_db.is_impedance_measure_on,  sizeof(cap_db.is_impedance_measure_on)},
    [DB_IF_CAP_AUTO_ACCEL_PULSING_MODE] = { &cap_db.is_auto_accel_pulsing_on, sizeof(cap_db.is_auto_accel_pulsing_on)},

};


void db_cap_init(void) {
    memset(&cap_db, 0, sizeof(db_cap_data_t));

    cap_db.hv_state = CAP_IS_IDLE;
    cap_db.hv_volt_set = 2;

    cap_db.lv_state = CAP_IS_IDLE;
    cap_db.lv_volt_set = 2;

}

bool db_cap_read(db_cap_id_t id, void* out_data) {

    if (id >= DB_ID_CAP_MAX || out_data == NULL) return false;

    memcpy(out_data, db_map[id].data_ptr, db_map[id].data_size);

    return true;
}

bool db_cap_write(db_cap_id_t id, const void* in_data) {

    if (id >= DB_ID_CAP_MAX || in_data == NULL)   return false;

    memcpy(db_map[id].data_ptr, in_data, db_map[id].data_size);

    return true;
}
