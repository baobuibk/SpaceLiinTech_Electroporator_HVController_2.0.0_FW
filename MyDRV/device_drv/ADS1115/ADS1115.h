/*
 * ADS1115.h
 *
 *  Created on: May 8, 2026
 *      Author: PV
 */

#ifndef DEVICE_DRV_ADS1115_ADS1115_H_
#define DEVICE_DRV_ADS1115_ADS1115_H_

#include "i2c.h"
#include <stdint.h>
#include <stdbool.h>

#define ADS1115_ADDRESS_GND     0x48    /* ADDR nối GND*/
#define ADS1115_ADDRESS_VDD     0x49    /* ADDR nối VDD*/
#define ADS1115_ADDRESS_SDA     0x4A    /* ADDR nối SDA*/
#define ADS1115_ADDRESS_SCL     0x4B    /* ADDR nối SCL*/

/* ========================================================================== */
/* REG ADDRESS                                       */
/* ========================================================================== */
#define ADS1115_REG_CONVERSION  0x00
#define ADS1115_REG_CONFIG      0x01
#define ADS1115_REG_LO_THRESH   0x02
#define ADS1115_REG_HI_THRESH   0x03


#define ADS1115_OS_START_SINGLE 0x8000
#define ADS1115_OS_BUSY         0x0000
#define ADS1115_OS_READY        0x8000

typedef enum {
    ADS1115_MUX_AIN0_GND  = 0x4000,
    ADS1115_MUX_AIN1_GND  = 0x5000,
    ADS1115_MUX_AIN2_GND  = 0x6000,
    ADS1115_MUX_AIN3_GND  = 0x7000
} ADS1115_MUX_t;


typedef enum {
    ADS1115_PGA_6_144V = 0x0000, // ±6.144V
    ADS1115_PGA_4_096V = 0x0200, // ±4.096V
    ADS1115_PGA_2_048V = 0x0400, // ±2.048V
    ADS1115_PGA_1_024V = 0x0600, // ±1.024V
    ADS1115_PGA_0_512V = 0x0800, // ±0.512V
    ADS1115_PGA_0_256V = 0x0A00  // ±0.256V
} ADS1115_PGA_t;


typedef enum {
    ADS1115_MODE_CONTINUOUS  = 0x0000, // Continuous-conversion mode
    ADS1115_MODE_SINGLE_SHOT = 0x0100  // Single-shot mode
} ADS1115_MODE_t;


typedef enum {
    ADS1115_DR_8SPS   = 0x0000,
    ADS1115_DR_16SPS  = 0x0020,
    ADS1115_DR_32SPS  = 0x0040,
    ADS1115_DR_64SPS  = 0x0060,
    ADS1115_DR_128SPS = 0x0080,
    ADS1115_DR_250SPS = 0x00A0,
    ADS1115_DR_475SPS = 0x00C0,
    ADS1115_DR_860SPS = 0x00E0
} ADS1115_DR_t;


typedef struct {
    I2C_Handle_t 	*dev_i2c;
    uint8_t 		dev_address;
    ADS1115_PGA_t 	dev_pga;
    ADS1115_DR_t	dev_dr;
    ADS1115_MODE_t	dev_mode;
    ADS1115_MUX_t	dev_channel;

    int16_t 		dev_raw_val[4];
    float			dev_volt_val[4];

} ADS1115_Handle_t;



I2C_Status_t ADS1115_Init(ADS1115_Handle_t *hads, I2C_Handle_t *hi2c);

/**
 * @brief Cấu hình IC ADS1115
 */
I2C_Status_t ADS1115_SetConfig(ADS1115_Handle_t *hads, ADS1115_MUX_t mux, ADS1115_PGA_t pga, ADS1115_MODE_t mode, ADS1115_DR_t dr);

/**
 * @brief Gửi lệnh bắt đầu lấy mẫu (khi dùng chế độ Single-Shot)
 */
I2C_Status_t ADS1115_StartSingleConversion(ADS1115_Handle_t *hads);

/**
 * @brief Kiểm tra xem IC đã hoàn tất chuyển đổi chưa
 */
I2C_Status_t ADS1115_IsReady(ADS1115_Handle_t *hads, bool *is_ready);

/**
 * @brief Đọc giá trị ADC RAW trả về (16-bit)
 */
I2C_Status_t ADS1115_ReadRawValue(ADS1115_Handle_t *hads, int16_t *raw_value);

/**
 * @brief Đọc giá trị ADC và chuyển đổi sang Voltage dựa vào cấu hình PGA
 */
I2C_Status_t ADS1115_ReadVoltage(ADS1115_Handle_t *hads, float *voltage);

#endif /* DEVICE_DRV_ADS1115_ADS1115_H_ */
