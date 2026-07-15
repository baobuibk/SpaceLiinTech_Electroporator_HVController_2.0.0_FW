/*
 * h3lis331dl.c
 *
 *  Created on: Apr 18, 2025
 *      Author: HTSANG (Rewritten for I2C Handle structure)
 */

#include "h3lis331dl.h"


static void H3LIS331DL_Write(H3LIS331DL_Handle_t *hh3lis, uint8_t reg, uint8_t data)
{
    hh3lis->status = I2C_Write(hh3lis->dev_i2c, H3LIS331DL_I2C_ADDR, reg, data);
}

static void H3LIS331DL_Read(H3LIS331DL_Handle_t *hh3lis, uint8_t reg, uint8_t *pData, uint8_t length)
{
    hh3lis->status = I2C_ReadMulti(hh3lis->dev_i2c, H3LIS331DL_I2C_ADDR, reg, pData, length);
}


/* ==========================================================
 *  API Khởi tạo & Thông số
 * ========================================================== */

I2C_Status_t H3LIS331DL_Init(H3LIS331DL_Handle_t *hh3lis, I2C_Handle_t *hi2c)
{
    hh3lis->dev_i2c = hi2c;
    hh3lis->status = I2C_Success;
    hh3lis->sensitivity = H3LIS_SENSITIVITY_400G;

    H3LIS331DL_Write(hh3lis, H3LIS331DL_CTRL_REG1, 0x3F);       // Normal mode, 1000Hz, enable XYZ
    H3LIS331DL_Write(hh3lis, H3LIS331DL_CTRL_REG2, 0x20);
    H3LIS331DL_Write(hh3lis, H3LIS331DL_CTRL_REG4, 0x80);

    H3LIS331DL_Write(hh3lis, H3LIS331DL_INT1_CFG, 0x00);
    H3LIS331DL_Write(hh3lis, H3LIS331DL_INT1_THS, 127);

    H3LIS331DL_Set_FS(hh3lis, H3LIS331DL_FS_400G);

    uint8_t ID = 0;
    H3LIS331DL_Read_ID(hh3lis, &ID);
    if (ID != H3LIS331DL_ID) {
        hh3lis->status = I2C_Error;
    }

    return hh3lis->status;
}

I2C_Status_t H3LIS331DL_Read_ID(H3LIS331DL_Handle_t *hh3lis, uint8_t *ID)
{
    H3LIS331DL_Read(hh3lis, H3LIS331DL_ID_REG, ID, 1);  // ID Device: 0x32
    return hh3lis->status;
}

I2C_Status_t H3LIS331DL_Set_FS(H3LIS331DL_Handle_t *hh3lis, H3LIS331DL_FS_t fs)
{
    uint8_t ctrl_reg4 = 0;
    H3LIS331DL_Read(hh3lis, H3LIS331DL_CTRL_REG4, &ctrl_reg4, 1);

    if (hh3lis->status != I2C_Success) return hh3lis->status;

    // Xóa bit 4 và 5 (0011 0000 = 0x30) sau đó set giá trị FS mới
    ctrl_reg4 &= ~(0x30);
    ctrl_reg4 |= (uint8_t)fs;
    H3LIS331DL_Write(hh3lis, H3LIS331DL_CTRL_REG4, ctrl_reg4);

    // Tự động cập nhật hệ số phân giải theo FS
    if (fs == H3LIS331DL_FS_100G) hh3lis->sensitivity = H3LIS_SENSITIVITY_100G;
    else if (fs == H3LIS331DL_FS_200G) hh3lis->sensitivity = H3LIS_SENSITIVITY_200G;
    else if (fs == H3LIS331DL_FS_400G) hh3lis->sensitivity = H3LIS_SENSITIVITY_400G;

    return hh3lis->status;
}

H3LIS331DL_FS_t H3LIS331DL_Get_FS(H3LIS331DL_Handle_t *hh3lis)
{
    uint8_t ctrl_reg4 = 0;
    H3LIS331DL_Read(hh3lis, H3LIS331DL_CTRL_REG4, &ctrl_reg4, 1);
    if (hh3lis->status == I2C_Success) {
        return (H3LIS331DL_FS_t)(ctrl_reg4 & 0x30);
    }
    return H3LIS331DL_FS_400G; // Default fallback
}


/* ==========================================================
 *  API Đọc dữ liệu Polling (Blocking)
 * ========================================================== */

I2C_Status_t H3LIS331DL_Get_Accel(H3LIS331DL_Handle_t *hh3lis)
{
    if (hh3lis->status == I2C_Error) {
        H3LIS331DL_Init(hh3lis, hh3lis->dev_i2c);
    }

    H3LIS331DL_Read(hh3lis, H3LIS331DL_OUT_X_L_MULTI, hh3lis->accel_raw, 6);

    if (hh3lis->status == I2C_Success) {
        int16_t x_raw = (int16_t)((hh3lis->accel_raw[1] << 8) | hh3lis->accel_raw[0]);
        int16_t y_raw = (int16_t)((hh3lis->accel_raw[3] << 8) | hh3lis->accel_raw[2]);
        int16_t z_raw = (int16_t)((hh3lis->accel_raw[5] << 8) | hh3lis->accel_raw[4]);

        // Shift 4 bits như datasheet
        hh3lis->acceleration[0] = x_raw >> 4;
        hh3lis->acceleration[1] = y_raw >> 4;
        hh3lis->acceleration[2] = z_raw >> 4;

        // Nhân với sensitivity để ra mg
        hh3lis->Data.x = (float)(hh3lis->acceleration[0]) * hh3lis->sensitivity;
        hh3lis->Data.y = (float)(hh3lis->acceleration[1]) * hh3lis->sensitivity;
        hh3lis->Data.z = (float)(hh3lis->acceleration[2]) * hh3lis->sensitivity;
    }

    return hh3lis->status;
}


/* ==========================================================
 *  API Đọc dữ liệu Interrupt (Non-Blocking)
 * ========================================================== */

I2C_Status_t H3LIS331DL_Get_Accel_IT_Start(H3LIS331DL_Handle_t *hh3lis)
{
    if (hh3lis->dev_i2c->State == I2C_STATE_IDLE) {
        hh3lis->status = I2C_Read_IT(hh3lis->dev_i2c, H3LIS331DL_I2C_ADDR, H3LIS331DL_OUT_X_L_MULTI, hh3lis->accel_raw, 6);
    } else {
        hh3lis->status = I2C_Busy;
    }
    return hh3lis->status;
}

bool H3LIS331DL_Get_Accel_IT_Complete(H3LIS331DL_Handle_t *hh3lis)
{
    if (hh3lis->dev_i2c->State == I2C_STATE_IDLE) {

        if (hh3lis->dev_i2c->Status == I2C_Success) {
            int16_t x_raw = (int16_t)((hh3lis->accel_raw[1] << 8) | hh3lis->accel_raw[0]);
            int16_t y_raw = (int16_t)((hh3lis->accel_raw[3] << 8) | hh3lis->accel_raw[2]);
            int16_t z_raw = (int16_t)((hh3lis->accel_raw[5] << 8) | hh3lis->accel_raw[4]);

            hh3lis->acceleration[0] = x_raw >> 4;
            hh3lis->acceleration[1] = y_raw >> 4;
            hh3lis->acceleration[2] = z_raw >> 4;

            hh3lis->Data.x = (float)(hh3lis->acceleration[0]) * hh3lis->sensitivity;
            hh3lis->Data.y = (float)(hh3lis->acceleration[1]) * hh3lis->sensitivity;
            hh3lis->Data.z = (float)(hh3lis->acceleration[2]) * hh3lis->sensitivity;

            return true;
        } else if (hh3lis->dev_i2c->Status == I2C_Error) {
            hh3lis->status = I2C_Error;
        }
    }
    return false;
}


/* ==========================================================
 *  API Cấu hình Ngắt Gia tốc
 * ========================================================== */

I2C_Status_t H3LIS331DL_Set_Interrupt_Threshold(H3LIS331DL_Handle_t *hh3lis, float threshold_g)
{
    uint8_t raw_ths;

    if (hh3lis->sensitivity == H3LIS_SENSITIVITY_100G)
        raw_ths = (uint8_t)((threshold_g * 128.0f) / 100.0f);
    else if (hh3lis->sensitivity == H3LIS_SENSITIVITY_200G)
        raw_ths = (uint8_t)((threshold_g * 128.0f) / 200.0f);
    else if (hh3lis->sensitivity == H3LIS_SENSITIVITY_400G)
        raw_ths = (uint8_t)((threshold_g * 128.0f) / 400.0f);

    if (raw_ths > 127) raw_ths = 127;

    H3LIS331DL_Write(hh3lis, H3LIS331DL_INT1_THS, raw_ths);
    return hh3lis->status;
}

I2C_Status_t H3LIS331DL_Get_Interrupt_Threshold(H3LIS331DL_Handle_t *hh3lis, float *threshold_g)
{
    float current_fs_g = 0.0f;
    uint8_t raw_ths = 0;

    H3LIS331DL_Read(hh3lis, H3LIS331DL_INT1_THS, &raw_ths, 1);

    if (hh3lis->sensitivity == H3LIS_SENSITIVITY_100G)
        current_fs_g = 100.0f;
    else if (hh3lis->sensitivity == H3LIS_SENSITIVITY_200G)
        current_fs_g = 200.0f;
    else if (hh3lis->sensitivity == H3LIS_SENSITIVITY_400G)
        current_fs_g = 400.0f;
    
    if (hh3lis->status == I2C_Success) {
        raw_ths &= 0x7F;
        *threshold_g = ((float)raw_ths * current_fs_g) / 128.0f;
    }

    return hh3lis->status;
}

I2C_Status_t H3LIS331DL_Enable_INT1_All(H3LIS331DL_Handle_t *hh3lis, bool int_enable)
{
    if (int_enable) {
        H3LIS331DL_Write(hh3lis, H3LIS331DL_INT1_CFG, 0x2A);

        uint8_t ctrl3 = 0;
        H3LIS331DL_Read(hh3lis, H3LIS331DL_CTRL_REG3, &ctrl3, 1);

        ctrl3 &= ~(0x03);
        ctrl3 |= (1 << 7) | (1 << 6);

        H3LIS331DL_Write(hh3lis, H3LIS331DL_CTRL_REG3, ctrl3);
    }
    else {
        H3LIS331DL_Write(hh3lis, H3LIS331DL_INT1_CFG, 0x00);
    }

    return hh3lis->status;
}
