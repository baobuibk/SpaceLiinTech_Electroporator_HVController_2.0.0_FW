

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "ads1115.h"
#include "board.h"
#include "i2c.h"


static float ADS1115_Cof_Cal(uint8_t ADS1115_pga){

	switch (ADS1115_pga) {

	case ADS1115_PGA_TWOTHIRDS:
		return 0.1875;

	case ADS1115_PGA_ONE:
		return 0.125;

	case ADS1115_PGA_TWO:
		return 0.0625;

	case ADS1115_PGA_FOUR:
		return 0.03125;

	case ADS1115_PGA_EIGHT:
		return 0.015625;

	case ADS1115_PGA_SIXTEEN:
		return 0.0078125;

	}
}

static void ADS1115_Val_Cal(ADS_1115_handle* p_handle){

    p_handle->data.raw_value = (int16_t)(
        ((uint16_t)p_handle->rx_buf[0] << 8) |
        ((uint16_t)p_handle->rx_buf[1])
    );

    float coef = ADS1115_Cof_Cal(p_handle -> config.pga);
    p_handle->data.voltage_mV = ((int32_t)p_handle->data.raw_value * coef)*1000;
}

void ADS1115_Init_Handle(ADS_1115_handle* p_handle)
{
    if (p_handle == NULL)
    {
        return;
    }

    p_handle->state        = ADS1115_STATE_RESET;
    p_handle->busy_count   = 0;
    p_handle->write_flag   = I2C_IS_RUNNING;
    p_handle->read_flag    = I2C_IS_RUNNING;
    p_handle->result       = I2C_IS_RUNNING;

    memset(p_handle->tx_buf,  0, sizeof(p_handle->tx_buf));
    memset(p_handle->rx_buf,  0, sizeof(p_handle->rx_buf));
    memset(&p_handle->data,   0, sizeof(p_handle->data));

}



i2c_result_t ADS1115_Read_Value(i2c_stdio_typedef* p_i2c, ADS_1115_handle* p_handle){

	i2c_result_t return_value;

    if ((p_i2c == NULL) || (p_handle == NULL))
    {
        return I2C_FAIL;
    }

    if (LL_I2C_IsActiveFlag_BUSY(p_i2c->handle) == 1)
    {
        p_handle->busy_count++;

        if (p_handle->busy_count < ADS1115_BUS_BUSY_LIMIT)
        {
            return I2C_IS_RUNNING;
        }


        p_handle->busy_count = 0;
        p_handle->state      = ADS1115_STATE_RESET;
        p_handle->result     = I2C_ERROR_BUS_BUSY;
        return I2C_ERROR_BUS_BUSY;
    }

    p_handle->busy_count = 0;


    switch (p_handle->state)
    {
    case ADS1115_STATE_RESET:
    {
        memset(p_handle->rx_buf, 0, sizeof(p_handle->rx_buf));

        p_handle -> tx_buf[0] = 0;
        p_handle -> tx_buf[0] |= ADS1115_OS|p_handle->config.input_channel | p_handle->config.pga | ADS1115_MODE_SINGLE;

        p_handle -> tx_buf[1] = 0;
        p_handle -> tx_buf[1] |= p_handle->config.data_rate;

        p_handle->state = ADS1115_STATE_TRIGGER_CONV;

        return I2C_IS_RUNNING;
    }

    case ADS1115_STATE_TRIGGER_CONV:
    {
    	I2C_Mem_Write_IT(p_i2c, ADS1115_ADDR, ADS1115_CONFIG_REG,p_handle->tx_buf, 2 , &p_handle->write_flag);

        p_handle->state = ADS1115_STATE_WAIT_WRITE_DONE;

        return I2C_IS_RUNNING;
    }

    case ADS1115_STATE_WAIT_WRITE_DONE:
    {
         return_value = Is_I2C_Write_Complete(&p_handle->write_flag);

         if (return_value != I2C_OK)
         {
            break;
         }

         I2C_Mem_Read_IT(p_i2c, ADS1115_ADDR, ADS1115_CONFIG_REG, p_handle -> rx_buf, 2, &p_handle->read_flag);
         p_handle->state = ADS1115_STATE_POLL_OS;
         return I2C_IS_RUNNING;
    }

    case ADS1115_STATE_POLL_OS:
    {
        return_value = Is_I2C_Read_Complete(&p_handle->read_flag);
        if (return_value != I2C_OK)
        {
            break;
        }

        if ((p_handle->rx_buf[0] & ADS1115_OS) == 0u)
        {
        	I2C_Mem_Read_IT(p_i2c, ADS1115_ADDR, ADS1115_CONFIG_REG, p_handle -> rx_buf, 2, &p_handle->read_flag);

            return I2C_IS_RUNNING;
        }

        I2C_Mem_Read_IT(p_i2c, ADS1115_ADDR, ADS1115_CONVER_REG, p_handle -> rx_buf, 2, &p_handle->read_flag);

        p_handle->state = ADS1115_STATE_FETCH_DATA;
        return I2C_IS_RUNNING;
    }
    case ADS1115_STATE_FETCH_DATA:
    {
    	return_value = Is_I2C_Read_Complete(&p_handle->read_flag);
        if (return_value != I2C_OK)
        {
            break;
        }

        p_handle->state = ADS1115_STATE_PROCESS;
        return I2C_IS_RUNNING;
    }
    case ADS1115_STATE_PROCESS:
    {
        ADS1115_Val_Cal(p_handle);

        p_handle->state  = ADS1115_STATE_RESET;
        p_handle->result = I2C_OK;
        return I2C_OK;
    }
    default:
    {
        p_handle->state = ADS1115_STATE_RESET;
        return I2C_IS_RUNNING;
    }

    }

    if (return_value!= I2C_IS_RUNNING)
    {
        p_handle->state  = ADS1115_STATE_RESET;
        p_handle->result = return_value;

        return return_value;
    }

    return I2C_IS_RUNNING;

}

bool ADS1115_Is_Read_Complete(ADS_1115_handle* p_handle)
{
    if (p_handle == NULL)
    {
        return false;
    }

    if (p_handle->result == I2C_OK)
    {
        p_handle->result = I2C_IS_RUNNING;     /* one-shot: clear after reading */
        return true;
    }

    return false;
}


