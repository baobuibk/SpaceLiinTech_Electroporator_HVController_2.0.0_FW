/*
 * h3lis331dl.h
 *
 *  Created on: Apr 18, 2025
 *      Author: HTSANG (Rewritten for I2C Handle structure)
 */

#ifndef SENSOR_I2C_H3LIS331DL_H3LIS331DL_H_
#define SENSOR_I2C_H3LIS331DL_H3LIS331DL_H_

#include "main.h"
#include "i2c.h"
#include "stdbool.h"
#include "stdint.h"

#define H3LIS331DL_ID                       0x32
#define H3LIS331DL_ID_REG                   0x0F

/** I2C Device Address 8 bit format  if SA0=0 -> 0x18 if SA0=1 -> 0x19 **/
#define H3LIS331DL_I2C_ADDR                 0x19

#define H3LIS331DL_CTRL_REG1                0x20
#define H3LIS331DL_CTRL_REG2                0x21
#define H3LIS331DL_CTRL_REG3                0x22
#define H3LIS331DL_CTRL_REG4                0x23
#define H3LIS331DL_OUT_X_L                  0x28
#define H3LIS331DL_OUT_X_L_MULTI            0xA8

#define H3LIS331DL_INT1_CFG                 0x30
#define H3LIS331DL_INT1_THS                 0x32

#define H3LIS_SENSITIVITY_100G              49.0f
#define H3LIS_SENSITIVITY_200G              98.0f
#define H3LIS_SENSITIVITY_400G              195.0f

// Khai báo Enum cấu hình mức Full Scale cho IC
typedef enum {
    H3LIS331DL_FS_100G = 0x00, // FS bits: 00
    H3LIS331DL_FS_200G = 0x10, // FS bits: 01
    H3LIS331DL_FS_400G = 0x30  // FS bits: 11
} H3LIS331DL_FS_t;

typedef enum {
    H3LIS331DL_AXIS_X = 0,
    H3LIS331DL_AXIS_Y = 1,
    H3LIS331DL_AXIS_Z = 2
} H3LIS331DL_Axis_t;

typedef enum {
    H3LIS331DL_INT_HIGH = 0,
    H3LIS331DL_INT_LOW = 1
} H3LIS331DL_IntCondition_t;

typedef struct {
    float x;
    float y;
    float z;
} H3LIS331DL_Data_t;

typedef struct {
    I2C_Handle_t        *dev_i2c;       // Handle I2C
    uint8_t             accel_raw[6];   // Buffer đọc dữ liệu gốc
    int16_t             acceleration[3];
    float               sensitivity;    // Hệ số chuyển đổi mg/digit hiện hành
    H3LIS331DL_Data_t   Data;           // Dữ liệu gia tốc (mg) cuối cùng
    I2C_Status_t        status;         // Trạng thái giao tiếp
} H3LIS331DL_Handle_t;



I2C_Status_t H3LIS331DL_Init(H3LIS331DL_Handle_t *hh3lis, I2C_Handle_t *hi2c);
I2C_Status_t H3LIS331DL_Read_ID(H3LIS331DL_Handle_t *hh3lis, uint8_t *ID);

I2C_Status_t H3LIS331DL_Get_Accel(H3LIS331DL_Handle_t *hh3lis);
I2C_Status_t H3LIS331DL_Get_Accel_IT_Start(H3LIS331DL_Handle_t *hh3lis);
bool H3LIS331DL_Get_Accel_IT_Complete(H3LIS331DL_Handle_t *hh3lis);

I2C_Status_t H3LIS331DL_Set_FS(H3LIS331DL_Handle_t *hh3lis, H3LIS331DL_FS_t fs);
H3LIS331DL_FS_t H3LIS331DL_Get_FS(H3LIS331DL_Handle_t *hh3lis);

I2C_Status_t H3LIS331DL_Set_Interrupt_Threshold(H3LIS331DL_Handle_t *hh3lis, float threshold_g);
I2C_Status_t H3LIS331DL_Get_Interrupt_Threshold(H3LIS331DL_Handle_t *hh3lis, float *threshold_g);
I2C_Status_t H3LIS331DL_Enable_INT1_All(H3LIS331DL_Handle_t *hh3lis, bool enable);

#endif /* SENSOR_I2C_H3LIS331DL_H3LIS331DL_H_ */
