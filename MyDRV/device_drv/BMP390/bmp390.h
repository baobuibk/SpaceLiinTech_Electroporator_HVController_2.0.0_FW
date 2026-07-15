/*
 * bmp390.h
 *
 * Created on: Dec 16, 2024
 * Author: SANG HUYNH
 * Updated: Rewritten for object-oriented I2C Handle structure
 */

#ifndef DEVICES_BMP390_BMP390_H_
#define DEVICES_BMP390_BMP390_H_

#include "i2c.h"
#include <stdbool.h>

#define BMP390_SEA_LEVEL_PRESSURE    101000.0f
#define BMP390_I2C_ADDR              0x77

#define BMP390_CALIB_DATA_ADDR       0x31
#define BMP390_TEMP_PRESS_DATA_ADDR  0x04
#define BMP390_REG_PWR_CTRL          0x1B

// Calibration data structure for BMP390
typedef struct {
    uint16_t    u16_NVM_T1;
    uint16_t    u16_NVM_T2;
    int8_t      i8_NVM_T3;
    int16_t     i16_NVM_P1;
    int16_t     i16_NVM_P2;
    int8_t      i8_NVM_P3;
    int8_t      i8_NVM_P4;
    uint16_t    u16_NVM_P5;
    uint16_t    u16_NVM_P6;
    int8_t      i8_NVM_P7;
    int8_t      i8_NVM_P8;
    int16_t     i16_NVM_P9;
    int8_t      i8_NVM_P10;
    int8_t      i8_NVM_P11;
} BMP390_Raw_Calib_Data_t;

typedef struct {
    float   f_PAR_T1;
    float   f_PAR_T2;
    float   f_PAR_T3;
    float   f_PAR_P1;
    float   f_PAR_P2;
    float   f_PAR_P3;
    float   f_PAR_P4;
    float   f_PAR_P5;
    float   f_PAR_P6;
    float   f_PAR_P7;
    float   f_PAR_P8;
    float   f_PAR_P9;
    float   f_PAR_P10;
    float   f_PAR_P11;
} BMP390_Calib_Data_t;

typedef enum {
    BMP390_MODE_SLEEP   = 0,
    BMP390_MODE_FORCED  = 1,
    BMP390_MODE_NORMAL  = 3
} BMP390_Mode;

typedef enum {
    BMP390_SUCCESS              = 0,
    BMP390_ERROR_UNKNOWN        = 1,
    BMP390_ERROR_SET_MODE       = 2,
    BMP390_ERROR_READ_CALIB     = 3,
    BMP390_ERROR_READ_TEMPRESS  = 4
} BMP390_ERROR;


typedef struct {
    I2C_Handle_t                *dev_i2c;       // Handle I2C giao tiếp
    BMP390_Raw_Calib_Data_t     NVM;
    BMP390_Calib_Data_t         PAR;

    int32_t                     temperature_raw;
    int32_t                     pressure_raw;

    float                       temperature;    // Compensated Temperature (Celsius)
    float                       pressure;       // Compensated Pressure (Pa)
    float                       altitude;       // Calculated Altitude (m)

    BMP390_ERROR                error;
    uint8_t                     raw_buffer[6];  // Buffer để nhận dữ liệu ngắt
} BMP390_Handle_t;


bool BMP390_init(BMP390_Handle_t *hbmp, I2C_Handle_t *hi2c);
void BMP390_set_mode(BMP390_Handle_t *hbmp, BMP390_Mode mode);

void bmp390_temp_press_update(BMP390_Handle_t *hbmp);

void bmp390_temp_press_update_IT_Start(BMP390_Handle_t *hbmp);
bool bmp390_temp_press_update_IT_Complete(BMP390_Handle_t *hbmp);

double bmp390_get_temperature(BMP390_Handle_t *hbmp);
double bmp390_get_press(BMP390_Handle_t *hbmp);
double bmp390_get_altitude(BMP390_Handle_t *hbmp);

#endif /* DEVICES_BMP390_BMP390_H_ */
