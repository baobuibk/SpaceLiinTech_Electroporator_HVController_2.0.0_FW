/*
 * bmp390.h
 *
 * Created on: Dec 16, 2024
 * Author: SANG HUYNH
 * Updated: Fixed initialization logic and double getters
 */

#ifndef DEVICES_BMP390_BMP390_H_
#define DEVICES_BMP390_BMP390_H_

#include "main.h"
#include "i2c.h"
#include <stdbool.h>

// Standard Sea Level Pressure in Pascals (Pa)
//#define BMP390_SEA_LEVEL_PRESSURE    101325.0f
#define BMP390_SEA_LEVEL_PRESSURE    101000.0f

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

typedef struct _BMP390_Data_
{
    int32_t                     temperature_raw;
    float                       temperature;            // Compensated Temperature (Celsius)
    int32_t                     pressure_raw;
    float                       pressure;               // Compensated Pressure (Pa)
    float                       altitude;               // Calculated Altitude (m)
    uint8_t                     chipID;
    BMP390_Raw_Calib_Data_t     NVM;
    BMP390_Calib_Data_t         PAR;
    BMP390_ERROR                BMP390_ERR;
} BMP390_Data;

typedef struct _BMP390_Value_
{
    float                       temperature;
    float                       pressure;
    float                       altitude;

}BMP390_Value;



bool BMP390_init(void);
void BMP390_set_mode(BMP390_Mode mode);
void bmp390_temp_press_update(BMP390_Value* dev);

// Trả về số thực chuẩn, không cần scale
double bmp390_get_temperature(void);
double bmp390_get_press(void);
double bmp390_get_altitude(void);

#endif /* DEVICES_BMP390_BMP390_H_ */
