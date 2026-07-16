
#ifndef I2C_I2C_H_
#define I2C_I2C_H_

#include <stdint.h>
#include <stddef.h>
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_i2c.h"

#define I2C_TIMEOUT 4000

typedef enum {
    I2C_Success = 0,
    I2C_Error,
    I2C_Busy,
    I2C_Timeout
} I2C_Status_t;

typedef enum {
    I2C_ERROR_NONE    = 0x00,
    I2C_ERROR_BERR    = 0x01, /* Bus error - vi pham dieu kien START/STOP */
    I2C_ERROR_ARLO    = 0x02, /* Arbitration lost (multi-master) */
    I2C_ERROR_AF      = 0x04, /* Acknowledge Failure - slave khong ACK (NACK) */
    I2C_ERROR_OVR     = 0x08, /* Overrun/Underrun */
    I2C_ERROR_TIMEOUT = 0x10  /* Timeout khi cho flag phan cung */
} I2C_ErrorCode_t;

typedef enum {
    I2C_STATE_IDLE = 0,
    I2C_STATE_TX_MEM_ADDR,
    I2C_STATE_TX_DATA,
    I2C_STATE_RX_MEM_ADDR,
    I2C_STATE_RX_RESTART,
    I2C_STATE_RX_DATA
} I2C_State_t;


typedef struct {
    I2C_TypeDef *Instance;
    uint8_t  DevAddress;
    uint8_t  MemAddress;
    uint8_t  *pBuffer;
    uint16_t Size;
    uint16_t Count;
    volatile I2C_State_t  State;
    volatile I2C_Status_t Status;
    volatile uint32_t     ErrorCode;
}I2C_Handle_t;

/* ---------------- Khoi tao & dang ky callback ---------------- */
void I2C_Init(I2C_Handle_t *hi2c, I2C_TypeDef *instance);

I2C_Status_t I2C_Write(I2C_Handle_t *hi2c, uint8_t addr, uint8_t reg, uint8_t data);
I2C_Status_t I2C_Read(I2C_Handle_t *hi2c, uint8_t addr, uint8_t reg, uint8_t *pData);
I2C_Status_t I2C_ReadMulti(I2C_Handle_t *hi2c, uint8_t addr, uint8_t reg, uint8_t *pData, uint16_t size);
I2C_Status_t I2C_WriteMulti(I2C_Handle_t *hi2c, uint8_t addr, uint8_t reg, uint8_t *pData, uint16_t size);
I2C_Status_t I2C_IsDeviceReady(I2C_Handle_t *hi2c, uint8_t addr, uint32_t retries);

I2C_Status_t I2C_Write_IT(I2C_Handle_t *hi2c, uint8_t addr, uint8_t reg, uint8_t *pData, uint16_t size);
I2C_Status_t I2C_Read_IT(I2C_Handle_t *hi2c, uint8_t addr, uint8_t reg, uint8_t *pData, uint16_t size);

void I2C_EV_IRQHandler(I2C_Handle_t *hi2c);
void I2C_ER_IRQHandler(I2C_Handle_t *hi2c);



#endif /* I2C_I2C_H_ */
