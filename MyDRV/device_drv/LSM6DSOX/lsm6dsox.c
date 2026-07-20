/*
 * lsm6dsox.c
 *
 *  Created on: Apr 18, 2025
 *      Author: HTSANG (Rewritten for I2C Handle structure)
 */

#include "lsm6dsox.h"
#include "math.h"


static void LSM6DSOX_Write(LSM6DSOX_Handle_t *hlsm, uint8_t reg, uint8_t data) {
    hlsm->status = I2C_Write(hlsm->dev_i2c, LSM6DSOX_ADDRESS, reg, data);
}

static void LSM6DSOX_ReadMulti(LSM6DSOX_Handle_t *hlsm, uint8_t reg, uint8_t *pData, uint8_t length) {
    hlsm->status = I2C_ReadMulti(hlsm->dev_i2c, LSM6DSOX_ADDRESS, reg, pData, length);
}


I2C_Status_t LSM6DSOX_Init(LSM6DSOX_Handle_t *hlsm, I2C_Handle_t *hi2c) {
    hlsm->dev_i2c = hi2c;
    hlsm->status  = I2C_Success;

    LSM6DSOX_Write(hlsm, LSM6DSOX_CTRL1_XL, 0x50);  // Accelerometer: 104 Hz, 8g ~ 78.48 m/s^2
    LSM6DSOX_Write(hlsm, LSM6DSOX_CTRL2_G, 0x50);   // Gyroscope: 104 Hz, 500 dps

    uint8_t ID = 0;
    LSM6DSOX_Read_ID(hlsm, &ID);

    if (ID != LSM6DSOX_ID) {
        hlsm->status = I2C_Error;
    }
    return hlsm->status;
}


I2C_Status_t LSM6DSOX_Read_ID(LSM6DSOX_Handle_t *hlsm, uint8_t *ID) {
    LSM6DSOX_ReadMulti(hlsm, LSM6DSOX_ID_ADDR, ID, 1);
    return hlsm->status;
}

I2C_Status_t LSM6DSOX_Read_Data(LSM6DSOX_Handle_t *hlsm) {

//    if (hlsm->status == I2C_Error) {
//        LSM6DSOX_Init(hlsm, hlsm->dev_i2c);
//    }

    LSM6DSOX_ReadMulti(hlsm, LSM6DSOX_OUTX_L_G, hlsm->RxData, 12);

    int16_t gx = (int16_t)(hlsm->RxData[1] << 8 | hlsm->RxData[0]);
    int16_t gy = (int16_t)(hlsm->RxData[3] << 8 | hlsm->RxData[2]);
    int16_t gz = (int16_t)(hlsm->RxData[5] << 8 | hlsm->RxData[4]);

    int16_t ax = (int16_t)(hlsm->RxData[7] << 8 | hlsm->RxData[6]);
    int16_t ay = (int16_t)(hlsm->RxData[9] << 8 | hlsm->RxData[8]);
    int16_t az = (int16_t)(hlsm->RxData[11] << 8 | hlsm->RxData[10]);

    // Dữ liệu đầu ra Gyro chuyển đổi sang đơn vị dps
    hlsm->Data.Gyro.x = gx * GYRO_SENSITIVITY_250DPS;
    hlsm->Data.Gyro.y = gy * GYRO_SENSITIVITY_250DPS;
    hlsm->Data.Gyro.z = gz * GYRO_SENSITIVITY_250DPS;

    // Dữ liệu đầu ra Accel chuyển đổi sang đơn vị mg
    hlsm->Data.Accel.x = ax * ACCEL_SENSITIVITY_2G;
    hlsm->Data.Accel.y = ay * ACCEL_SENSITIVITY_2G;
    hlsm->Data.Accel.z = az * ACCEL_SENSITIVITY_2G;

    return hlsm->status;
}

I2C_Status_t LSM6DSOX_Read_Data_IT(LSM6DSOX_Handle_t *hlsm) {
    if (hlsm->dev_i2c->State == I2C_STATE_IDLE) {
        hlsm->status = I2C_Read_IT(hlsm->dev_i2c, LSM6DSOX_ADDRESS, LSM6DSOX_OUTX_L_G, hlsm->RxData, 12);
    } else {
        hlsm->status = I2C_Busy;
    }
    return hlsm->status;
}


void LSM6DSOX_Process_Data_IT(LSM6DSOX_Handle_t *hlsm) {
    if (hlsm->dev_i2c->State == I2C_STATE_IDLE && hlsm->dev_i2c->Status == I2C_Success) {

        int16_t gx = (int16_t)(hlsm->RxData[1] << 8 | hlsm->RxData[0]);
        int16_t gy = (int16_t)(hlsm->RxData[3] << 8 | hlsm->RxData[2]);
        int16_t gz = (int16_t)(hlsm->RxData[5] << 8 | hlsm->RxData[4]);

        int16_t ax = (int16_t)(hlsm->RxData[7] << 8 | hlsm->RxData[6]);
        int16_t ay = (int16_t)(hlsm->RxData[9] << 8 | hlsm->RxData[8]);
        int16_t az = (int16_t)(hlsm->RxData[11] << 8 | hlsm->RxData[10]);

        // Tính toán Gyro (dps)
        hlsm->Data.Gyro.x = gx * GYRO_SENSITIVITY_250DPS;
        hlsm->Data.Gyro.y = gy * GYRO_SENSITIVITY_250DPS;
        hlsm->Data.Gyro.z = gz * GYRO_SENSITIVITY_250DPS;

        // Tính toán Accel (mg)
        hlsm->Data.Accel.x = ax * ACCEL_SENSITIVITY_2G;
        hlsm->Data.Accel.y = ay * ACCEL_SENSITIVITY_2G;
        hlsm->Data.Accel.z = az * ACCEL_SENSITIVITY_2G;
    }
}
