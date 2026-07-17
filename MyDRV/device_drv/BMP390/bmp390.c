/*
 * bmp390.c
 *
 * Created on: Dec 16, 2024
 * Author: SANG HUYNH
 * Updated: Return I2C_Status_t for easier debugging
 */

#include "bmp390.h"
#include <math.h>


static I2C_Status_t BMP390_read_raw_calibration(BMP390_Handle_t *hbmp)
{
    uint8_t calib[21];
    I2C_Status_t status = I2C_ReadMulti(hbmp->dev_i2c, BMP390_I2C_ADDR, BMP390_CALIB_DATA_ADDR, calib, 21);

    if (status != I2C_Success) {
        hbmp->error = BMP390_ERROR_READ_CALIB;
        return status;
    }

    hbmp->NVM.u16_NVM_T1 = ((uint16_t)calib[1] << 8) | (uint16_t)calib[0];
    hbmp->NVM.u16_NVM_T2 = ((uint16_t)calib[3] << 8) | (uint16_t)calib[2];
    hbmp->NVM.i8_NVM_T3  = calib[4];
    hbmp->NVM.i16_NVM_P1 = ((int16_t)calib[6] << 8) | (int16_t)calib[5];
    hbmp->NVM.i16_NVM_P2 = ((int16_t)calib[8] << 8) | (int16_t)calib[7];
    hbmp->NVM.i8_NVM_P3  = calib[9];
    hbmp->NVM.i8_NVM_P4  = calib[10];
    hbmp->NVM.u16_NVM_P5 = ((uint16_t)calib[12] << 8) | (uint16_t)calib[11];
    hbmp->NVM.u16_NVM_P6 = ((uint16_t)calib[14] << 8) | (uint16_t)calib[13];
    hbmp->NVM.i8_NVM_P7  = calib[15];
    hbmp->NVM.i8_NVM_P8  = calib[16];
    hbmp->NVM.i16_NVM_P9 = ((int16_t)calib[18] << 8) | (int16_t)calib[17];
    hbmp->NVM.i8_NVM_P10 = calib[19];
    hbmp->NVM.i8_NVM_P11 = calib[20];

    return I2C_Success;
}

static void BMP390_convert_calibration(BMP390_Handle_t *hbmp)
{
    hbmp->PAR.f_PAR_T1 = hbmp->NVM.u16_NVM_T1 * 256.0f;
    hbmp->PAR.f_PAR_T2 = hbmp->NVM.u16_NVM_T2 / 1073741824.0f;
    hbmp->PAR.f_PAR_T3 = hbmp->NVM.i8_NVM_T3 / 281474976710656.0f;

    hbmp->PAR.f_PAR_P1 = (hbmp->NVM.i16_NVM_P1 - 16384) / 1048576.0f;
    hbmp->PAR.f_PAR_P2 = (hbmp->NVM.i16_NVM_P2 - 16384) / 536870912.0f;
    hbmp->PAR.f_PAR_P3 = hbmp->NVM.i8_NVM_P3 / 4294967296.0f;
    hbmp->PAR.f_PAR_P4 = hbmp->NVM.i8_NVM_P4 / 137438953472.0f;
    hbmp->PAR.f_PAR_P5 = hbmp->NVM.u16_NVM_P5 * 8.0f;
    hbmp->PAR.f_PAR_P6 = hbmp->NVM.u16_NVM_P6 / 64.0f;
    hbmp->PAR.f_PAR_P7 = hbmp->NVM.i8_NVM_P7 / 256.0f;
    hbmp->PAR.f_PAR_P8 = hbmp->NVM.i8_NVM_P8 / 32768.0f;
    hbmp->PAR.f_PAR_P9 = hbmp->NVM.i16_NVM_P9 / 281474976710656.0f;
    hbmp->PAR.f_PAR_P10 = hbmp->NVM.i8_NVM_P10 / 281474976710656.0f;
    hbmp->PAR.f_PAR_P11 = hbmp->NVM.i8_NVM_P11 / 36893488147419103232.0f;
}

static void BMP390_compensate_temperature(BMP390_Handle_t *hbmp)
{
    float partial_data1 = (float)(hbmp->temperature_raw - hbmp->PAR.f_PAR_T1);
    float partial_data2 = (float)(partial_data1 * hbmp->PAR.f_PAR_T2);
    hbmp->temperature = partial_data2 + (partial_data1 * partial_data1) * hbmp->PAR.f_PAR_T3;
}

static void BMP390_compensate_pressure(BMP390_Handle_t *hbmp)
{
    float partial_data1, partial_data2, partial_data3, partial_data4;
    float partial_out1, partial_out2;

    partial_data1 = hbmp->PAR.f_PAR_P6 * hbmp->temperature;
    partial_data2 = hbmp->PAR.f_PAR_P7 * (hbmp->temperature * hbmp->temperature);
    partial_data3 = hbmp->PAR.f_PAR_P8 * (hbmp->temperature * hbmp->temperature * hbmp->temperature);
    partial_out1 = hbmp->PAR.f_PAR_P5 + partial_data1 + partial_data2 + partial_data3;

    partial_data1 = hbmp->PAR.f_PAR_P2 * hbmp->temperature;
    partial_data2 = hbmp->PAR.f_PAR_P3 * (hbmp->temperature * hbmp->temperature);
    partial_data3 = hbmp->PAR.f_PAR_P4 * (hbmp->temperature * hbmp->temperature * hbmp->temperature);
    partial_out2 = (float)hbmp->pressure_raw * (hbmp->PAR.f_PAR_P1 + partial_data1 + partial_data2 + partial_data3);

    partial_data1 = (float)hbmp->pressure_raw * (float)hbmp->pressure_raw;
    partial_data2 = hbmp->PAR.f_PAR_P9 + hbmp->PAR.f_PAR_P10 * hbmp->temperature;
    partial_data3 = partial_data1 * partial_data2;
    partial_data4 = ((float)hbmp->pressure_raw * (float)hbmp->pressure_raw * (float)hbmp->pressure_raw) * hbmp->PAR.f_PAR_P11;

    hbmp->pressure = partial_out1 + partial_out2 + partial_data3 + partial_data4;
}

static void BMP390_calculate_altitude(BMP390_Handle_t *hbmp)
{
    if (hbmp->pressure > 0) {
        hbmp->altitude = 44330.0f * (1.0f - powf(hbmp->pressure / BMP390_SEA_LEVEL_PRESSURE, 0.1903f));
    } else {
        hbmp->altitude = 0.0f;
    }
}


/* ==========================================================
 *  API Khởi tạo
 * ========================================================== */

I2C_Status_t BMP390_init(BMP390_Handle_t *hbmp, I2C_Handle_t *hi2c)
{
    hbmp->dev_i2c = hi2c;
    hbmp->error = BMP390_SUCCESS;

    I2C_Status_t status = BMP390_read_raw_calibration(hbmp);

    if (status == I2C_Success) {
        BMP390_convert_calibration(hbmp);
        status = BMP390_set_mode(hbmp, BMP390_MODE_NORMAL);
    }

    return status;
}

I2C_Status_t BMP390_set_mode(BMP390_Handle_t *hbmp, BMP390_Mode mode)
{
    uint8_t reg_value;
    I2C_Status_t status = I2C_Read(hbmp->dev_i2c, BMP390_I2C_ADDR, BMP390_REG_PWR_CTRL, &reg_value);

    if (status != I2C_Success) {
        hbmp->error = BMP390_ERROR_SET_MODE;
        return status;
    }

    reg_value &= ~(0x33);       // Xóa bit 5, 4 (enables) và bit 1, 0 (mode cũ)
    reg_value |= (0x03 << 4);   // Bật temp_en và press_en (0x3 << 4 = 0x30)
    reg_value |= (mode & 0x03); // Set Mode

    status = I2C_Write(hbmp->dev_i2c, BMP390_I2C_ADDR, BMP390_REG_PWR_CTRL, reg_value);
    if (status != I2C_Success) {
        hbmp->error = BMP390_ERROR_SET_MODE;
    }
    return status;
}


/* ==========================================================
 *  API Polling (Blocking)
 * ========================================================== */

I2C_Status_t bmp390_temp_press_update(BMP390_Handle_t *hbmp)
{
    uint8_t temp_press_data[6];
    I2C_Status_t status = I2C_ReadMulti(hbmp->dev_i2c, BMP390_I2C_ADDR, BMP390_TEMP_PRESS_DATA_ADDR, temp_press_data, 6);

    if (status != I2C_Success) {
        hbmp->error = BMP390_ERROR_READ_TEMPRESS;
        return status;
    }

    hbmp->temperature_raw = ((uint32_t)temp_press_data[5] << 16) | ((uint32_t)temp_press_data[4] << 8) | (uint32_t)temp_press_data[3];
    hbmp->pressure_raw    = ((uint32_t)temp_press_data[2] << 16) | ((uint32_t)temp_press_data[1] << 8) | (uint32_t)temp_press_data[0];

    hbmp->error = BMP390_SUCCESS;
    BMP390_compensate_temperature(hbmp);
    BMP390_compensate_pressure(hbmp);
    BMP390_calculate_altitude(hbmp);

    return I2C_Success;
}


/* ==========================================================
 *  API Interrupt (Non-Blocking cho Scheduler)
 * ========================================================== */

I2C_Status_t bmp390_temp_press_update_IT_Start(BMP390_Handle_t *hbmp)
{
    if (hbmp->dev_i2c->State == I2C_STATE_IDLE) {
        return I2C_Read_IT(hbmp->dev_i2c, BMP390_I2C_ADDR, BMP390_TEMP_PRESS_DATA_ADDR, hbmp->raw_buffer, 6);
    }
    return I2C_Busy;
}

I2C_Status_t bmp390_temp_press_update_IT_Complete(BMP390_Handle_t *hbmp)
{
    if (hbmp->dev_i2c->State == I2C_STATE_IDLE) {

        if (hbmp->dev_i2c->Status == I2C_Success) {
            hbmp->temperature_raw = ((uint32_t)hbmp->raw_buffer[5] << 16) | ((uint32_t)hbmp->raw_buffer[4] << 8) | (uint32_t)hbmp->raw_buffer[3];
            hbmp->pressure_raw    = ((uint32_t)hbmp->raw_buffer[2] << 16) | ((uint32_t)hbmp->raw_buffer[1] << 8) | (uint32_t)hbmp->raw_buffer[0];

            hbmp->error = BMP390_SUCCESS;
            BMP390_compensate_temperature(hbmp);
            BMP390_compensate_pressure(hbmp);
            BMP390_calculate_altitude(hbmp);

            return I2C_Success;
        }
        else if (hbmp->dev_i2c->Status == I2C_Error) {
            hbmp->error = BMP390_ERROR_READ_TEMPRESS;
            return I2C_Error;
        }
    }

    return I2C_Busy; // Nếu State != IDLE thì nghĩa là đang bận đọc
}


/* ==========================================================
 *  API Getters
 * ========================================================== */

double bmp390_get_temperature(BMP390_Handle_t *hbmp)
{
    if (hbmp->error != BMP390_SUCCESS) return -999.0;
    return (double)hbmp->temperature;
}

double bmp390_get_press(BMP390_Handle_t *hbmp)
{
    if (hbmp->error != BMP390_SUCCESS) return -999.0;
    return (double)hbmp->pressure;
}

double bmp390_get_altitude(BMP390_Handle_t *hbmp)
{
    if (hbmp->error != BMP390_SUCCESS) return -999.0;
    return (double)hbmp->altitude;
}
