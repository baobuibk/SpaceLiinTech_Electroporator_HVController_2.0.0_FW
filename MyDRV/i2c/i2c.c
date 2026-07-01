/*
 * i2c.c
 *
 * Created on: Nov 29, 2024
 * Author: SANG HUYNH
 */

#include "i2c.h"

I2C_Status_t I2C_Write(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t reg, uint8_t data)
{
    uint32_t timeout = I2C_TIMEOUT;

    // 1. Chờ I2C rảnh
    while (LL_I2C_IsActiveFlag_BUSY(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    // 2. Tạo điều kiện START
    LL_I2C_GenerateStartCondition(I2Cx);
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_SB(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    // 3. Gửi địa chỉ Slave (chế độ Write)
    LL_I2C_TransmitData8(I2Cx, (addr << 1));
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_ADDR(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;
    LL_I2C_ClearFlag_ADDR(I2Cx); // Phải xóa cờ ADDR để tiếp tục

    // 4. Gửi địa chỉ thanh ghi (reg)
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_TXE(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;
    LL_I2C_TransmitData8(I2Cx, reg);

    // 5. Gửi dữ liệu (data)
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_TXE(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;
    LL_I2C_TransmitData8(I2Cx, data);

    // 6. Chờ quá trình truyền dữ liệu hoàn tất (Byte Transfer Finished)
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_BTF(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    // 7. Tạo điều kiện STOP
    LL_I2C_GenerateStopCondition(I2Cx);

    return I2C_Success;
}


I2C_Status_t I2C_Read(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t reg, uint8_t* pData)
{
    uint32_t timeout = I2C_TIMEOUT;

    // --- GIAI ĐOẠN 1: Ghi địa chỉ thanh ghi cần đọc ---
    while (LL_I2C_IsActiveFlag_BUSY(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    LL_I2C_GenerateStartCondition(I2Cx);
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_SB(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    LL_I2C_TransmitData8(I2Cx, (addr << 1)); // Write
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_ADDR(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;
    LL_I2C_ClearFlag_ADDR(I2Cx);

    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_TXE(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;
    LL_I2C_TransmitData8(I2Cx, reg);
    
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_BTF(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    // --- GIAI ĐOẠN 2: Đọc dữ liệu (Repeated Start) ---
    LL_I2C_GenerateStartCondition(I2Cx);
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_SB(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    LL_I2C_TransmitData8(I2Cx, (addr << 1) | 1); // Read
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_ADDR(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    // Vô hiệu hóa ACK và tạo STOP ngay sau khi xóa cờ ADDR (vì chỉ đọc 1 byte)
    LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_NACK);
    LL_I2C_ClearFlag_ADDR(I2Cx);
    LL_I2C_GenerateStopCondition(I2Cx);

    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_RXNE(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    *pData = LL_I2C_ReceiveData8(I2Cx);

    // Kích hoạt lại ACK cho các lần giao tiếp sau
    LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);

    return I2C_Success;
}


I2C_Status_t I2C_ReadMulti(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t reg, uint8_t* pData, uint16_t size)
{
    uint32_t timeout = I2C_TIMEOUT;

    if (size == 0) return I2C_Error;

    // --- GIAI ĐOẠN 1: Ghi địa chỉ thanh ghi ---
    while (LL_I2C_IsActiveFlag_BUSY(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    LL_I2C_GenerateStartCondition(I2Cx);
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_SB(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    LL_I2C_TransmitData8(I2Cx, (addr << 1));
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_ADDR(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;
    LL_I2C_ClearFlag_ADDR(I2Cx);

    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_TXE(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;
    LL_I2C_TransmitData8(I2Cx, reg);

    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_BTF(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    // --- GIAI ĐOẠN 2: Đọc nhiều byte (Repeated Start) ---
    LL_I2C_GenerateStartCondition(I2Cx);
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_SB(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    LL_I2C_TransmitData8(I2Cx, (addr << 1) | 1); // Read
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_ADDR(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    if (size == 1) {
        // Đọc 1 byte: Xử lý giống như hàm I2C_Read
        LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_NACK);
        LL_I2C_ClearFlag_ADDR(I2Cx);
        LL_I2C_GenerateStopCondition(I2Cx);
    } else {
        // Bật ACK để đọc nhiều byte
        LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
        LL_I2C_ClearFlag_ADDR(I2Cx);
    }

    for (uint16_t i = 0; i < size; i++) {
        // NACK byte cuối cùng trước khi nó được nhận hoàn toàn
        if (i == (size - 1) && size > 1) {
            LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_NACK);
            LL_I2C_GenerateStopCondition(I2Cx);
        }

        timeout = I2C_TIMEOUT;
        while (!LL_I2C_IsActiveFlag_RXNE(I2Cx) && timeout--);
        if (timeout == 0) return I2C_Error;

        pData[i] = LL_I2C_ReceiveData8(I2Cx);
    }

    // Kích hoạt lại ACK mặc định sau khi hoàn thành
    LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);

    return I2C_Success;
}