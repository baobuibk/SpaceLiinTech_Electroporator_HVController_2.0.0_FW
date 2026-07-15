/*
 * lsm6dsox.h
 *
 *  Created on: Apr 18, 2025
 *      Author: HTSANG (Rewritten for I2C Handle structure)
 */

#ifndef SENSOR_I2C_LSM6DSOX_LSM6DSOX_H_
#define SENSOR_I2C_LSM6DSOX_LSM6DSOX_H_

#include "stdint.h"
#include "i2c.h"

#define LSM6DSOX_ID                 0x6C
#define LSM6DSOX_ID_ADDR            0x0F
#define LSM6DSOX_ADDRESS            0x6A

#define LSM6DSOX_CTRL1_XL           0X10
#define LSM6DSOX_CTRL2_G            0X11
#define LSM6DSOX_STATUS_REG         0X1E
#define LSM6DSOX_CTRL3_C            0X12
#define LSM6DSOX_CTRL6_C            0X15
#define LSM6DSOX_CTRL7_G            0X16
#define LSM6DSOX_CTRL8_XL           0X17
#define LSM6DSOX_OUTX_L_G           0X22

#define GYRO_SENSITIVITY_500DPS     0.01750f        // dps/LSB
#define ACCEL_SENSITIVITY_8G        0.000244f       // g/LSB
#define GYRO_SENSITIVITY_250DPS     0.00875f        // dps/LSB
#define ACCEL_SENSITIVITY_2G        0.061f          // mg/LSB

typedef struct {
    float x;
    float y;
    float z;
} Accel_Gyro_t;

typedef struct {
    Accel_Gyro_t Accel;
    Accel_Gyro_t Gyro;
} LSM6DSOX_Data_t;

typedef struct {
    I2C_Handle_t    *dev_i2c;       // Con trỏ trỏ tới Handle I2C (hi2c)
    uint8_t         RxData[12];     // Mảng chứa dữ liệu raw
    I2C_Status_t    status;         // Trạng thái I2C
    LSM6DSOX_Data_t Data;           // Dữ liệu Accel và Gyro đã được convert
} LSM6DSOX_Handle_t;

/* ---------------- API ---------------- */
I2C_Status_t LSM6DSOX_Init(LSM6DSOX_Handle_t *hlsm, I2C_Handle_t *hi2c);
I2C_Status_t LSM6DSOX_Read_ID(LSM6DSOX_Handle_t *hlsm, uint8_t *ID);
I2C_Status_t LSM6DSOX_Read_Data(LSM6DSOX_Handle_t *hlsm);

I2C_Status_t LSM6DSOX_Read_Data_IT(LSM6DSOX_Handle_t *hlsm);
void LSM6DSOX_Process_Data_IT(LSM6DSOX_Handle_t *hlsm);

#endif /* SENSOR_I2C_LSM6DSOX_LSM6DSOX_H_ */
