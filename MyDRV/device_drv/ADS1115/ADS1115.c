/*
 * ADS1115.c
 *
 *  Created on: May 8, 2026
 *      Author: PV
 */

#include "ads1115.h"



static float ADS1115_GetVoltageMultiplier(ADS1115_PGA_t pga);



I2C_Status_t ADS1115_Init(ADS1115_Handle_t *hads, I2C_Handle_t *hi2c) {
    if (hads == NULL || hi2c == NULL) return I2C_Error;

    hads->dev_i2c = hi2c;
    hads->dev_address = ADS1115_ADDRESS_GND;
    hads->dev_pga = ADS1115_PGA_2_048V;

    return I2C_IsDeviceReady(hads->dev_i2c, hads->dev_address, 3);
}

I2C_Status_t ADS1115_SetConfig(ADS1115_Handle_t *hads, ADS1115_MUX_t mux, ADS1115_PGA_t pga, ADS1115_MODE_t mode, ADS1115_DR_t dr) {
    if (hads == NULL) return I2C_Error;

    hads->dev_pga = pga;
    hads->dev_channel = mux;
    hads->dev_mode = mode;
    hads->dev_dr = dr;

    uint16_t config = mux | pga | mode | dr | 0x0003; // 0x0003 cho COMP_QUE=11 (Disable)[cite: 3]

    uint8_t data[2];
    data[0] = (uint8_t)(config >> 8);
    data[1] = (uint8_t)(config & 0xFF);

    return I2C_WriteMulti(hads->dev_i2c, hads->dev_address, ADS1115_REG_CONFIG, data, 2);
}

I2C_Status_t ADS1115_StartSingleConversion(ADS1115_Handle_t *hads) {
    if (hads == NULL) return I2C_Error;

    uint8_t data[2];

    if (I2C_ReadMulti(hads->dev_i2c, hads->dev_address, ADS1115_REG_CONFIG, data, 2) != I2C_Success) {
        return I2C_Error;
    }

    data[0] |= 0x80;

    return I2C_WriteMulti(hads->dev_i2c, hads->dev_address, ADS1115_REG_CONFIG, data, 2);
}

I2C_Status_t ADS1115_IsReady(ADS1115_Handle_t *hads, bool *is_ready) {
    if (hads == NULL || is_ready == NULL) return I2C_Error;

    uint8_t data[2];
    if (I2C_ReadMulti(hads->dev_i2c, hads->dev_address, ADS1115_REG_CONFIG, data, 2) != I2C_Success) {
        return I2C_Error;
    }

    if ((data[0] & 0x80) != 0) {
        *is_ready = true;
    } else {
        *is_ready = false;
    }

    return I2C_Success;
}

I2C_Status_t ADS1115_ReadRawValue(ADS1115_Handle_t *hads, int16_t *raw_value) {
    if (hads == NULL || raw_value == NULL) return I2C_Error;

    uint8_t data[2];
    /* Đọc 2 byte từ thanh ghi Conversion (0x00)*/
    if (I2C_ReadMulti(hads->dev_i2c, hads->dev_address, ADS1115_REG_CONVERSION, data, 2) != I2C_Success) {
        return I2C_Error;
    }

    /* ADS1115 trả về định dạng 16-bit 2's-complement, MSB nằm trước*/
    *raw_value = (int16_t)((data[0] << 8) | data[1]);

    return I2C_Success;
}

I2C_Status_t ADS1115_ReadVoltage(ADS1115_Handle_t *hads, float *voltage) {
    if (hads == NULL || voltage == NULL) return I2C_Error;

    int16_t raw_value = 0;
    I2C_Status_t status = ADS1115_ReadRawValue(hads, &raw_value);

    if (status == I2C_Success) {
        float multiplier = ADS1115_GetVoltageMultiplier(hads->dev_pga);
        *voltage = (float)raw_value * multiplier;
    }

    return status;
}


static float ADS1115_GetVoltageMultiplier(ADS1115_PGA_t pga) {
    switch (pga) {
        case ADS1115_PGA_6_144V: return 0.1875f;    // LSB Size: 187.5 uV[cite: 3]
        case ADS1115_PGA_4_096V: return 0.125f;     // LSB Size: 125 uV[cite: 3]
        case ADS1115_PGA_2_048V: return 0.0625f;    // LSB Size: 62.5 uV[cite: 3]
        case ADS1115_PGA_1_024V: return 0.03125f;   // LSB Size: 31.25 uV[cite: 3]
        case ADS1115_PGA_0_512V: return 0.015625f;  // LSB Size: 15.625 uV[cite: 3]
        case ADS1115_PGA_0_256V: return 0.0078125f; // LSB Size: 7.8125 uV[cite: 3]
        default:                 return 0.0625f;
    }
}
