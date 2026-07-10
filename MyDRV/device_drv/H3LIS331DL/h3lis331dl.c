/*
 * h3lis331dl.c
 *
 *  Created on: Apr 18, 2025
 *      Author: HTSANG
 */

#include "h3lis331dl.h"


H3LIS331DL_RawData_t H3LIS331DL_RawData = {
		.sensitivity = H3LIS_SENSITIVITY_400G, 
		.status = I2C_Success,
};

void H3LIS331DL_Write(uint8_t reg, uint8_t data)
{
	H3LIS331DL_RawData.status = I2C_Write(ONBOARD_SENSOR_I2C_HANDLE, H3LIS331DL_I2C_ADDR, reg, data);
}

void H3LIS331DL_Read(uint8_t reg, uint8_t *pData, uint8_t length)
{
	H3LIS331DL_RawData.status = I2C_ReadMulti(ONBOARD_SENSOR_I2C_HANDLE, H3LIS331DL_I2C_ADDR, reg, pData, length);
}

I2C_Status_t H3LIS331DL_Set_FS(H3LIS331DL_FS_t fs)
{
	uint8_t ctrl_reg4 = 0;
	// Đọc giá trị thanh ghi hiện tại để bảo toàn các bit cấu hình khác (BDU, BLE, v.v.)
	H3LIS331DL_Read(H3LIS331DL_CTRL_REG4, &ctrl_reg4, 1);
	if (H3LIS331DL_RawData.status != I2C_Success) return H3LIS331DL_RawData.status;

	// Xóa bit 4 và 5 (0011 0000 = 0x30) sau đó set giá trị FS mới
	ctrl_reg4 &= ~(0x30);
	ctrl_reg4 |= (uint8_t)fs;
	H3LIS331DL_Write(H3LIS331DL_CTRL_REG4, ctrl_reg4);

	// Tự động cập nhật hệ số phân giải theo FS
	if (fs == H3LIS331DL_FS_100G) H3LIS331DL_RawData.sensitivity = H3LIS_SENSITIVITY_100G;
	else if (fs == H3LIS331DL_FS_200G) H3LIS331DL_RawData.sensitivity = H3LIS_SENSITIVITY_200G;
	else if (fs == H3LIS331DL_FS_400G) H3LIS331DL_RawData.sensitivity = H3LIS_SENSITIVITY_400G;

	return H3LIS331DL_RawData.status;
}

H3LIS331DL_FS_t H3LIS331DL_Get_FS(void)
{
	uint8_t ctrl_reg4 = 0;
	H3LIS331DL_Read(H3LIS331DL_CTRL_REG4, &ctrl_reg4, 1);
	if (H3LIS331DL_RawData.status == I2C_Success) {
		return (H3LIS331DL_FS_t)(ctrl_reg4 & 0x30);
	}

	return H3LIS331DL_FS_400G; // Default value
}

I2C_Status_t H3LIS331DL_Init(void)
{
	H3LIS331DL_Write(H3LIS331DL_CTRL_REG1, 0x3F);		// 0011 1111 (Normal mode, 1000Hz, enable XYZ)
	H3LIS331DL_Write(H3LIS331DL_CTRL_REG2, 0x20);
	H3LIS331DL_Write(H3LIS331DL_CTRL_REG4, 0x80);

	H3LIS331DL_Write(H3LIS331DL_INT1_CFG, 0x00);
	H3LIS331DL_Write(H3LIS331DL_INT1_THS, 127);
	
	H3LIS331DL_Set_FS(H3LIS331DL_FS_400G);

	uint8_t ID = 0;
	H3LIS331DL_Read_ID(&ID);
	if (ID != H3LIS331DL_ID) H3LIS331DL_RawData.status = I2C_Error;
	return H3LIS331DL_RawData.status;
}

I2C_Status_t H3LIS331DL_Read_ID(uint8_t *ID)
{
	H3LIS331DL_Read(H3LIS331DL_ID_REG, ID, 1);  //ID Device: 0x32
	return H3LIS331DL_RawData.status;
}

void H3LIS331DL_Read_RawData(H3LIS331DL_RawData_t *RawData)
{
	H3LIS331DL_Read(H3LIS331DL_OUT_X_L_MULTI, RawData->accel_raw, 6);

	int16_t x_raw = (int16_t)((RawData->accel_raw[1] << 8) | RawData->accel_raw[0]);
	int16_t y_raw = (int16_t)((RawData->accel_raw[3] << 8) | RawData->accel_raw[2]);
	int16_t z_raw = (int16_t)((RawData->accel_raw[5] << 8) | RawData->accel_raw[4]);

	RawData->acceleration[0] = x_raw >> 4;
	RawData->acceleration[1] = y_raw >> 4;
	RawData->acceleration[2] = z_raw >> 4;
}

I2C_Status_t H3LIS331DL_Get_Accel(H3LIS331DL_Data_t *Data)
{
	if (H3LIS331DL_RawData.status == I2C_Error) H3LIS331DL_Init();
	H3LIS331DL_Read_RawData(&H3LIS331DL_RawData);

	// Nhân với sensitivity được cập nhật tương ứng từ hàm H3LIS331DL_Set_FS()
	Data->x = (float)(H3LIS331DL_RawData.acceleration[0]) * H3LIS331DL_RawData.sensitivity;
	Data->y = (float)(H3LIS331DL_RawData.acceleration[1]) * H3LIS331DL_RawData.sensitivity;
	Data->z = (float)(H3LIS331DL_RawData.acceleration[2]) * H3LIS331DL_RawData.sensitivity;
	
	return H3LIS331DL_RawData.status;
}

/**
 * @brief  Cài đặt ngưỡng gia tốc cho bộ tạo ngắt 1
 * @param  threshold_g: Giá trị ngưỡng mong muốn (đơn vị g, VD: 50.0f)
 * @param  current_fs_g: Thang đo FS đang dùng để làm hệ quy chiếu (VD: 100.0f, 200.0f, 400.0f)
 */
I2C_Status_t H3LIS331DL_Set_Interrupt_Threshold(float threshold_g)
{
    // Thanh ghi INT1_THS lưu giá trị 7-bit (0 đến 127)
	uint8_t raw_ths;

	if(H3LIS331DL_RawData.sensitivity == H3LIS_SENSITIVITY_100G)
		raw_ths = (uint8_t)((threshold_g * 128.0f) / 100.0f);
	else if(H3LIS331DL_RawData.sensitivity == H3LIS_SENSITIVITY_200G)
		raw_ths = (uint8_t)((threshold_g * 128.0f) / 200.0f);
	else if(H3LIS331DL_RawData.sensitivity == H3LIS_SENSITIVITY_400G)
		raw_ths = (uint8_t)((threshold_g * 128.0f) / 400.0f);

    if (raw_ths > 127) raw_ths = 127;

    H3LIS331DL_Write(H3LIS331DL_INT1_THS, raw_ths);
    return H3LIS331DL_RawData.status;
}

/**
 * @brief  Đọc giá trị ngưỡng gia tốc đang được cài đặt trong IC
 * @param  threshold_g: Con trỏ để lưu giá trị ngưỡng đọc được (đơn vị g)
 * @param  current_fs_g: Thang đo FS đang dùng để làm hệ quy chiếu (VD: 100.0f, 200.0f, 400.0f)
 */
I2C_Status_t H3LIS331DL_Get_Interrupt_Threshold(float *threshold_g)
{
	float current_fs_g = 0.0f;
    uint8_t raw_ths = 0;

    H3LIS331DL_Read(H3LIS331DL_INT1_THS, &raw_ths, 1);

	if(H3LIS331DL_RawData.sensitivity == H3LIS_SENSITIVITY_100G)
		current_fs_g = 100.0f;
	else if(H3LIS331DL_RawData.sensitivity == H3LIS_SENSITIVITY_200G)
		current_fs_g = 200.0f;
	else if(H3LIS331DL_RawData.sensitivity == H3LIS_SENSITIVITY_400G)
		current_fs_g = 400.0f;
    
    if (H3LIS331DL_RawData.status == I2C_Success) {
        raw_ths &= 0x7F;
        *threshold_g = ((float)raw_ths * current_fs_g) / 128.0f;
    }

    return H3LIS331DL_RawData.status;
}

/**
 * @brief  Bật/Tắt chế độ: Bất kỳ trục nào vượt ngưỡng cũng tạo ngắt
 * @param  enable: true (Bật) hoặc false (Tắt)
 */
I2C_Status_t H3LIS331DL_Enable_INT1_All(bool int_enable)
{
    if (int_enable) {
        H3LIS331DL_Write(H3LIS331DL_INT1_CFG, 0x2A);

        uint8_t ctrl3 = 0;
        H3LIS331DL_Read(H3LIS331DL_CTRL_REG3, &ctrl3, 1);

        ctrl3 &= ~(0x03);
        ctrl3 |= (1 << 7) | (1 << 6);

        H3LIS331DL_Write(H3LIS331DL_CTRL_REG3, ctrl3);
    }
    else {
        H3LIS331DL_Write(H3LIS331DL_INT1_CFG, 0x00);
    }
	
    return H3LIS331DL_RawData.status;
}

/**
 * @brief  Cấu hình ngắt linh hoạt cho 1 trục duy nhất
 * @param  axis: Trục cần theo dõi (X, Y hoặc Z)
 * @param  cond: Điều kiện ngắt (HIGH - vượt ngưỡng, LOW - tụt dưới ngưỡng)
 */
//I2C_Status_t H3LIS331DL_Set_Int1_SingleAxis(H3LIS331DL_Axis_t axis, H3LIS331DL_IntCondition_t cond)
//{
//    uint8_t cfg_val = 0;
//
//    if (axis == H3LIS331DL_AXIS_X) {
//        cfg_val = (cond == H3LIS331DL_INT_HIGH) ? (1 << 1) : (1 << 0);
//    }
//    else if (axis == H3LIS331DL_AXIS_Y) {
//        cfg_val = (cond == H3LIS331DL_INT_HIGH) ? (1 << 3) : (1 << 2);
//    }
//    else if (axis == H3LIS331DL_AXIS_Z) {
//        cfg_val = (cond == H3LIS331DL_INT_HIGH) ? (1 << 5) : (1 << 4);
//    }
//
//    H3LIS331DL_Write(H3LIS331DL_INT1_CFG, cfg_val);
//
//    uint8_t ctrl3 = 0;
//    H3LIS331DL_Read(H3LIS331DL_CTRL_REG3, &ctrl3, 1);
//    ctrl3 &= ~(0x03);
//    H3LIS331DL_Write(H3LIS331DL_CTRL_REG3, ctrl3);
//
//    return H3LIS331DL_RawData.status;
//}
