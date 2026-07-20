/*
 * i2c.c
 *
 * Created on: Nov 29, 2024
 * Author: SANG HUYNH
 *
 * Nang cap: hoan thien duong TX cua I2C_EV_IRQHandler, them I2C_ER_IRQHandler,
 * callback tang tren, I2C_WriteMulti, I2C_IsDeviceReady, I2C_BusRecovery.
 */

#include "i2c.h"

#include "scheduler.h"

static void I2C_Delay_us(void)
{
    __IO uint32_t wait = 100;
    while(wait--) {
        __NOP();
    }
}

void I2C_Init(I2C_Handle_t *hi2c, I2C_TypeDef *instance)
{
    hi2c->Instance    = instance;
    hi2c->DevAddress  = 0;
    hi2c->MemAddress  = 0;
    hi2c->pBuffer     = NULL;
    hi2c->Size        = 0;
    hi2c->Count       = 0;
    hi2c->State       = I2C_STATE_IDLE;
    hi2c->Status      = I2C_Success;
    hi2c->ErrorCode   = I2C_ERROR_NONE;

    LL_I2C_DisableIT_EVT(instance);
    LL_I2C_DisableIT_ERR(instance);
    LL_I2C_DisableIT_BUF(instance);
}


I2C_Status_t I2C_Write(I2C_Handle_t *hi2c, uint8_t addr, uint8_t reg, uint8_t data)
{
    uint32_t timeout = I2C_TIMEOUT;

    // 1. Cho Bus ranh
    while (LL_I2C_IsActiveFlag_BUSY(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }

    // 2. Tao dieu kien START
    LL_I2C_GenerateStartCondition(hi2c->Instance);
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_SB(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }

    // 3. Gui dia chi Slave (che do Write)
    LL_I2C_TransmitData8(hi2c->Instance, (addr << 1));
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_ADDR(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }
    LL_I2C_ClearFlag_ADDR(hi2c->Instance); // Phai xoa co ADDR de tiep tuc

    // 4. Gui dia chi thanh ghi (reg)
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_TXE(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }
    LL_I2C_TransmitData8(hi2c->Instance, reg);

    // 5. Gui du lieu (data)
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_TXE(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }
    LL_I2C_TransmitData8(hi2c->Instance, data);

    // 6. Cho qua trinh truyen du lieu hoan tat (Byte Transfer Finished)
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_BTF(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }

    // 7. Tao dieu kien STOP
    LL_I2C_GenerateStopCondition(hi2c->Instance);

    return I2C_Success;
}


I2C_Status_t I2C_Read(I2C_Handle_t *hi2c, uint8_t addr, uint8_t reg, uint8_t* pData)
{
    uint32_t timeout = I2C_TIMEOUT;

    // --- GIAI DOAN 1: Ghi dia chi thanh ghi can doc ---
    while (LL_I2C_IsActiveFlag_BUSY(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }

    LL_I2C_GenerateStartCondition(hi2c->Instance);
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_SB(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }

    LL_I2C_TransmitData8(hi2c->Instance, (addr << 1)); // Write
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_ADDR(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }
    LL_I2C_ClearFlag_ADDR(hi2c->Instance);

    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_TXE(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }
    LL_I2C_TransmitData8(hi2c->Instance, reg);

    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_BTF(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }

    // --- GIAI DOAN 2: Doc du lieu (Repeated Start) ---
    LL_I2C_GenerateStartCondition(hi2c->Instance);
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_SB(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }

    LL_I2C_TransmitData8(hi2c->Instance, (addr << 1) | 1); // Read
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_ADDR(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }

    // Vo hieu hoa ACK va tao STOP ngay sau khi xoa co ADDR (vi chi doc 1 byte)
    LL_I2C_AcknowledgeNextData(hi2c->Instance, LL_I2C_NACK);
    LL_I2C_ClearFlag_ADDR(hi2c->Instance);
    LL_I2C_GenerateStopCondition(hi2c->Instance);

    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_RXNE(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }

    *pData = LL_I2C_ReceiveData8(hi2c->Instance);

    LL_I2C_AcknowledgeNextData(hi2c->Instance, LL_I2C_ACK);

    return I2C_Success;
}


I2C_Status_t I2C_ReadMulti(I2C_Handle_t *hi2c, uint8_t addr, uint8_t reg, uint8_t* pData, uint16_t size)
{
    uint32_t timeout = I2C_TIMEOUT;

    if (size == 0) return I2C_Error;

    // --- GIAI DOAN 1: Ghi dia chi thanh ghi ---
    while (LL_I2C_IsActiveFlag_BUSY(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }

    LL_I2C_GenerateStartCondition(hi2c->Instance);
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_SB(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }

    LL_I2C_TransmitData8(hi2c->Instance, (addr << 1));
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_ADDR(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }
    LL_I2C_ClearFlag_ADDR(hi2c->Instance);

    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_TXE(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }
    LL_I2C_TransmitData8(hi2c->Instance, reg);

    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_BTF(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }

    // --- GIAI DOAN 2: Doc nhieu byte (Repeated Start) ---
    LL_I2C_GenerateStartCondition(hi2c->Instance);
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_SB(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }

    LL_I2C_TransmitData8(hi2c->Instance, (addr << 1) | 1); // Read
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_ADDR(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }

    if (size == 1) {
        // Doc 1 byte: xu ly giong nhu ham I2C_Read
        LL_I2C_AcknowledgeNextData(hi2c->Instance, LL_I2C_NACK);
        LL_I2C_ClearFlag_ADDR(hi2c->Instance);
        LL_I2C_GenerateStopCondition(hi2c->Instance);
    } else {
        // Bat ACK de doc nhieu byte
        LL_I2C_AcknowledgeNextData(hi2c->Instance, LL_I2C_ACK);
        LL_I2C_ClearFlag_ADDR(hi2c->Instance);
    }

    for (uint16_t i = 0; i < size; i++) {
        // NACK byte cuoi cung truoc khi no duoc nhan hoan toan
        if (i == (size - 1) && size > 1) {
            LL_I2C_AcknowledgeNextData(hi2c->Instance, LL_I2C_NACK);
            LL_I2C_GenerateStopCondition(hi2c->Instance);
        }

        timeout = I2C_TIMEOUT;
        while (!LL_I2C_IsActiveFlag_RXNE(hi2c->Instance)) {
            if (--timeout == 0) return I2C_Error;
        }

        pData[i] = LL_I2C_ReceiveData8(hi2c->Instance);
    }

    LL_I2C_AcknowledgeNextData(hi2c->Instance, LL_I2C_ACK);

    return I2C_Success;
}


I2C_Status_t I2C_WriteMulti(I2C_Handle_t *hi2c, uint8_t addr, uint8_t reg, uint8_t* pData, uint16_t size)
{
    uint32_t timeout = I2C_TIMEOUT;

    if (size == 0 || pData == NULL) return I2C_Error;

    while (LL_I2C_IsActiveFlag_BUSY(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }

    LL_I2C_GenerateStartCondition(hi2c->Instance);
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_SB(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }

    LL_I2C_TransmitData8(hi2c->Instance, (addr << 1));
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_ADDR(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }
    LL_I2C_ClearFlag_ADDR(hi2c->Instance);

    // Dia chi thanh ghi bat dau
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_TXE(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }
    LL_I2C_TransmitData8(hi2c->Instance, reg);

    // Lan luot ghi tung byte du lieu (auto-increment o phia slave, theo dung
    // datasheet cua tung IC - gia dinh tuong tu I2C_ReadMulti)
    for (uint16_t i = 0; i < size; i++) {
        timeout = I2C_TIMEOUT;
        while (!LL_I2C_IsActiveFlag_TXE(hi2c->Instance)) {
            if (--timeout == 0) return I2C_Error;
        }
        LL_I2C_TransmitData8(hi2c->Instance, pData[i]);
    }

    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_BTF(hi2c->Instance)) {
        if (--timeout == 0) return I2C_Error;
    }

    LL_I2C_GenerateStopCondition(hi2c->Instance);

    return I2C_Success;
}

I2C_Status_t I2C_IsDeviceReady(I2C_Handle_t *hi2c, uint8_t addr, uint32_t retries)
{
    uint32_t timeout;

    timeout = I2C_TIMEOUT;
    while (LL_I2C_IsActiveFlag_BUSY(hi2c->Instance) && timeout--);
    if (timeout == 0) return I2C_Error;

    for (uint32_t attempt = 0; attempt <= retries; attempt++) {

        LL_I2C_GenerateStartCondition(hi2c->Instance);
        timeout = I2C_TIMEOUT;
        while (!LL_I2C_IsActiveFlag_SB(hi2c->Instance) && timeout--);
        if (timeout == 0) return I2C_Error;

        LL_I2C_TransmitData8(hi2c->Instance, (addr << 1));

        timeout = I2C_TIMEOUT;
        while (!LL_I2C_IsActiveFlag_ADDR(hi2c->Instance) && !LL_I2C_IsActiveFlag_AF(hi2c->Instance) && timeout--);
        if (timeout == 0) {
            LL_I2C_GenerateStopCondition(hi2c->Instance);
            return I2C_Error;
        }

        if (LL_I2C_IsActiveFlag_ADDR(hi2c->Instance)) {
            LL_I2C_ClearFlag_ADDR(hi2c->Instance);
            LL_I2C_GenerateStopCondition(hi2c->Instance);
            return I2C_Success; // Thiet bi da ACK dia chi -> co mat tren bus
        }

        // Bi NACK: xoa co AF, tao STOP roi thu lai (neu con luot retry)
        LL_I2C_ClearFlag_AF(hi2c->Instance);
        LL_I2C_GenerateStopCondition(hi2c->Instance);
    }

    return I2C_Error; // Khong thiet bi nao tra loi sau tat ca cac lan thu
}

void I2C_BusRecovery(I2C_Handle_t *hi2c,GPIO_TypeDef* SCL_Port, uint32_t SCL_Pin, GPIO_TypeDef* SDA_Port, uint32_t SDA_Pin)
{
	LL_I2C_Disable(hi2c->Instance);
	LL_I2C_DeInit(hi2c->Instance);

    LL_GPIO_SetPinMode(SCL_Port, SCL_Pin, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(SCL_Port, SCL_Pin, LL_GPIO_OUTPUT_OPENDRAIN);
    LL_GPIO_SetPinPull(SCL_Port, SCL_Pin, LL_GPIO_PULL_UP);

    LL_GPIO_SetPinMode(SDA_Port, SDA_Pin, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(SDA_Port, SDA_Pin, LL_GPIO_OUTPUT_OPENDRAIN);
    LL_GPIO_SetPinPull(SDA_Port, SDA_Pin, LL_GPIO_PULL_UP);

    LL_GPIO_SetOutputPin(SCL_Port, SCL_Pin);
    LL_GPIO_SetOutputPin(SDA_Port, SDA_Pin);
    I2C_Delay_us();

	for (uint8_t i = 0; i < 9; i++) {
		LL_GPIO_ResetOutputPin(SCL_Port, SCL_Pin);
		I2C_Delay_us();
		LL_GPIO_SetOutputPin(SCL_Port, SCL_Pin);
		I2C_Delay_us();
		if (LL_GPIO_IsInputPinSet(SDA_Port, SDA_Pin) != 0) {
			break;
		}
	}

    LL_GPIO_ResetOutputPin(SCL_Port, SCL_Pin);
    LL_GPIO_ResetOutputPin(SDA_Port, SDA_Pin);
    I2C_Delay_us();

    LL_GPIO_SetOutputPin(SCL_Port, SCL_Pin);
    I2C_Delay_us();

    LL_GPIO_SetOutputPin(SDA_Port, SDA_Pin);
    I2C_Delay_us();

    LL_GPIO_SetPinMode(SCL_Port, SCL_Pin, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinMode(SDA_Port, SDA_Pin, LL_GPIO_MODE_ALTERNATE);

//    hi2c->Instance->CR1 |= I2C_CR1_SWRST;
//    I2C_Delay_us();
//    hi2c->Instance->CR1 &= ~I2C_CR1_SWRST;

    hi2c->Status      = I2C_Success;
    hi2c->ErrorCode   = I2C_ERROR_NONE;

}

/* ============================================================
 *  API non-blocking (interrupt-driven)
 * ============================================================ */

I2C_Status_t I2C_Write_IT(I2C_Handle_t *hi2c, uint8_t addr, uint8_t reg, uint8_t *pData, uint16_t size)
{
    if (hi2c->State != I2C_STATE_IDLE) return I2C_Busy;
    if (LL_I2C_IsActiveFlag_BUSY(hi2c->Instance)) return I2C_Busy;
    if (size == 0 || pData == NULL) return I2C_Error;

    hi2c->DevAddress = addr;
    hi2c->MemAddress = reg;
    hi2c->pBuffer    = pData;
    hi2c->Size       = size;
    hi2c->Count      = size;
    hi2c->State      = I2C_STATE_TX_MEM_ADDR;
    hi2c->Status     = I2C_Busy;
    hi2c->ErrorCode  = I2C_ERROR_NONE;

    // Can bat ca BUF de co ngat TXE (nguon phat cac byte du lieu ghi)
    LL_I2C_EnableIT_EVT(hi2c->Instance);
    LL_I2C_EnableIT_ERR(hi2c->Instance);
    LL_I2C_EnableIT_BUF(hi2c->Instance);

    LL_I2C_GenerateStartCondition(hi2c->Instance);

    return I2C_Success;
}

I2C_Status_t I2C_Read_IT(I2C_Handle_t *hi2c, uint8_t addr, uint8_t reg, uint8_t *pData, uint16_t size)
{
    if (hi2c->State != I2C_STATE_IDLE) return I2C_Busy;
    if (LL_I2C_IsActiveFlag_BUSY(hi2c->Instance)) return I2C_Busy;
    if (size == 0 || pData == NULL) return I2C_Error;

    hi2c->DevAddress = addr;
    hi2c->MemAddress = reg;
    hi2c->pBuffer    = pData;
    hi2c->Size       = size;
    hi2c->Count      = size;
    hi2c->State      = I2C_STATE_RX_MEM_ADDR;
    hi2c->Status     = I2C_Busy;
    hi2c->ErrorCode  = I2C_ERROR_NONE;

    LL_I2C_EnableIT_EVT(hi2c->Instance);
    LL_I2C_EnableIT_ERR(hi2c->Instance);
    LL_I2C_EnableIT_BUF(hi2c->Instance);

    LL_I2C_GenerateStartCondition(hi2c->Instance);

    return I2C_Success;
}

void I2C_EV_IRQHandler(I2C_Handle_t *hi2c) {
	I2C_TypeDef *I2Cx = hi2c->Instance;

	// ========================================================
	// 1. XỬ LÝ CỜ SB (Start Bit)
	// ========================================================
	if (LL_I2C_IsActiveFlag_SB(I2Cx)) {
		if (hi2c->State == I2C_STATE_RX_MEM_ADDR || hi2c->State == I2C_STATE_TX_MEM_ADDR) {
			LL_I2C_TransmitData8(I2Cx, (hi2c->DevAddress << 1));
		} else if (hi2c->State == I2C_STATE_RX_RESTART) {
			LL_I2C_TransmitData8(I2Cx, (hi2c->DevAddress << 1) | 1);
			hi2c->State = I2C_STATE_RX_DATA;
		}
	}

	// ========================================================
	// 2. XỬ LÝ CỜ ADDR (Address matched)
	// ========================================================
	if (LL_I2C_IsActiveFlag_ADDR(I2Cx)) {

		if (hi2c->State == I2C_STATE_RX_DATA) {
			if (hi2c->Count == 1) {
				LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_NACK);
				LL_I2C_ClearFlag_ADDR(I2Cx);
				LL_I2C_GenerateStopCondition(I2Cx);
			} else if (hi2c->Count == 2) {
				LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_NACK);
				I2Cx->CR1 |= I2C_CR1_POS;
				LL_I2C_ClearFlag_ADDR(I2Cx);
				LL_I2C_DisableIT_BUF(I2Cx); // FIX LỖI SPAM NGẮT: Tắt BUF để chờ BTF
			} else {
				LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
				LL_I2C_ClearFlag_ADDR(I2Cx);
			}
		} else {
			LL_I2C_ClearFlag_ADDR(I2Cx);
			if (hi2c->State == I2C_STATE_RX_MEM_ADDR
					|| hi2c->State == I2C_STATE_TX_MEM_ADDR) {
				LL_I2C_TransmitData8(I2Cx, hi2c->MemAddress);
			}
		}
	}

	// ========================================================
	// 3. XỬ LÝ CỜ TXE (Transmit buffer empty)
	// ========================================================
	if (LL_I2C_IsActiveFlag_TXE(I2Cx) && !LL_I2C_IsActiveFlag_BTF(I2Cx)) {
		if (hi2c->State == I2C_STATE_TX_MEM_ADDR) {
			hi2c->State = I2C_STATE_TX_DATA;
			if (hi2c->Count > 0) {
				LL_I2C_TransmitData8(I2Cx, *hi2c->pBuffer);
				hi2c->pBuffer++;
				hi2c->Count--;
				// FIX LỖI SPAM NGẮT TXE
				if (hi2c->Count == 0)
					LL_I2C_DisableIT_BUF(I2Cx);
			}
		} else if (hi2c->State == I2C_STATE_TX_DATA) {
			if (hi2c->Count > 0) {
				LL_I2C_TransmitData8(I2Cx, *hi2c->pBuffer);
				hi2c->pBuffer++;
				hi2c->Count--;
				// FIX LỖI SPAM NGẮT TXE
				if (hi2c->Count == 0)
					LL_I2C_DisableIT_BUF(I2Cx);
			}
		}
	}

	// ========================================================
	// 4. XỬ LÝ CỜ BTF (Byte Transfer Finished)
	// ========================================================
	if (LL_I2C_IsActiveFlag_BTF(I2Cx)) {
		if (hi2c->State == I2C_STATE_RX_MEM_ADDR) {
			hi2c->State = I2C_STATE_RX_RESTART;
			LL_I2C_GenerateStartCondition(I2Cx);
		} else if (hi2c->State == I2C_STATE_TX_DATA && hi2c->Count == 0) {
			LL_I2C_GenerateStopCondition(I2Cx);
			LL_I2C_DisableIT_EVT(I2Cx);
			LL_I2C_DisableIT_ERR(I2Cx);
			LL_I2C_DisableIT_BUF(I2Cx);
			hi2c->State = I2C_STATE_IDLE;
			hi2c->Status = I2C_Success;
		} else if (hi2c->State == I2C_STATE_RX_DATA) {
			if (hi2c->Count == 2) {
				LL_I2C_GenerateStopCondition(I2Cx);
				*hi2c->pBuffer++ = LL_I2C_ReceiveData8(I2Cx);
				hi2c->Count--;
				*hi2c->pBuffer++ = LL_I2C_ReceiveData8(I2Cx);
				hi2c->Count--;

				// FIX LỖI KẸT STATE: Bổ sung tắt ngắt và chuyển state về IDLE
				LL_I2C_DisableIT_EVT(I2Cx);
				LL_I2C_DisableIT_ERR(I2Cx);
				LL_I2C_DisableIT_BUF(I2Cx);
				hi2c->State = I2C_STATE_IDLE;
				hi2c->Status = I2C_Success;
			} else if (hi2c->Count == 3) {
				LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_NACK);
				*hi2c->pBuffer++ = LL_I2C_ReceiveData8(I2Cx);
				hi2c->Count--;
			}
		}
	}

	// ========================================================
	// 5. XỬ LÝ CỜ RXNE (Receive buffer not empty)
	// ========================================================
	if (LL_I2C_IsActiveFlag_RXNE(I2Cx) && !LL_I2C_IsActiveFlag_BTF(I2Cx)) {
		if (hi2c->State == I2C_STATE_RX_DATA) {
			if (hi2c->Count > 3) {
				*hi2c->pBuffer++ = LL_I2C_ReceiveData8(I2Cx);
				hi2c->Count--;

				// FIX LỖI SPAM NGẮT: Khi Count rớt xuống còn 3, tắt BUF để nhường cho BTF xử lý
				if (hi2c->Count == 3) {
					LL_I2C_DisableIT_BUF(I2Cx);
				}
			} else if (hi2c->Count == 1) {
				*hi2c->pBuffer++ = LL_I2C_ReceiveData8(I2Cx);
				hi2c->Count--;

				I2Cx->CR1 &= ~I2C_CR1_POS;

				LL_I2C_DisableIT_EVT(I2Cx);
				LL_I2C_DisableIT_ERR(I2Cx);
				LL_I2C_DisableIT_BUF(I2Cx);

				hi2c->State = I2C_STATE_IDLE;
				hi2c->Status = I2C_Success;
			}
		}
	}
}

void I2C_ER_IRQHandler(I2C_Handle_t *hi2c)
{
    I2C_TypeDef *I2Cx = hi2c->Instance;
    uint32_t err = I2C_ERROR_NONE;

    if (LL_I2C_IsActiveFlag_BERR(I2Cx)) {
        LL_I2C_ClearFlag_BERR(I2Cx);
        err |= I2C_ERROR_BERR;
    }
    if (LL_I2C_IsActiveFlag_ARLO(I2Cx)) {
        LL_I2C_ClearFlag_ARLO(I2Cx);
        err |= I2C_ERROR_ARLO;
    }
    if (LL_I2C_IsActiveFlag_AF(I2Cx)) {
        LL_I2C_ClearFlag_AF(I2Cx);
        err |= I2C_ERROR_AF;
    }
    if (LL_I2C_IsActiveFlag_OVR(I2Cx)) {
        LL_I2C_ClearFlag_OVR(I2Cx);
        err |= I2C_ERROR_OVR;
    }

//    if (err != I2C_ERROR_NONE) {
//        hi2c->ErrorCode = err;
//        hi2c->Status    = I2C_Error;
//        hi2c->State     = I2C_STATE_IDLE;
//
//        LL_I2C_DisableIT_EVT(I2Cx);
//        LL_I2C_DisableIT_ERR(I2Cx);
//        LL_I2C_DisableIT_BUF(I2Cx);
//
//        LL_I2C_GenerateStopCondition(I2Cx);
//
//    }
}


