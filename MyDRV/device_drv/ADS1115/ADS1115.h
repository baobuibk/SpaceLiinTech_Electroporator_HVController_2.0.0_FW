/*
 * ADS1115.h
 *
 *  Created on: May 8, 2026
 *      Author: PV
 */

#ifndef DEVICE_DRV_ADS1115_ADS1115_H_
#define DEVICE_DRV_ADS1115_ADS1115_H_

#include <stdint.h>
#include "i2c.h"


#define ADS1115_OS (0b1 << 7) // Default

#define ADS1115_MUX_AIN0 (0b100 << 4)		// Analog input 1
#define ADS1115_MUX_AIN1 (0b101 << 4)		// Analog input 2
#define ADS1115_MUX_AIN2 (0b110 << 4)		// Analog input 3
#define ADS1115_MUX_AIN3 (0b111 << 4)		// Analog input 4

#define ADS1115_PGA_TWOTHIRDS 	(0b000 << 1) 		// 2/3x Gain	-- 0.1875 mV by one bit		MAX: +- VDD + 0.3V
#define ADS1115_PGA_ONE			(0b001 << 1) 		// 1x Gain		-- 0.125 mV by one bit		MAX: +- VDD + 0.3V
#define ADS1115_PGA_TWO			(0b010 << 1) 		// 2x Gain		-- 0.0625 mV by one bit		MAX: +- 2.048 V
#define ADS1115_PGA_FOUR		(0b011 << 1) 		// 4x Gain		-- 0.03125 mV by one bit	MAX: +- 1.024 V
#define ADS1115_PGA_EIGHT		(0b100 << 1) 		// 8x Gain		-- 0.015625 mV by one bit	MAX: +- 0.512 V
#define ADS1115_PGA_SIXTEEN		(0b111 << 1) 		// 16x Gain		-- 0.0078125 mV by one bit	MAX: +- 0.256 V

#define ADS1115_MODE_SINGLE (0b1) // Default
#define ADS1115_MODE_CONTINOUS (0b0) // Default

#define ADS1115_DATA_RATE_8		(0b000 << 5)			// 8SPS
#define ADS1115_DATA_RATE_16	(0b001 << 5)			// 16SPS
#define ADS1115_DATA_RATE_32	(0b010 << 5)			// 32SPS
#define ADS1115_DATA_RATE_64	(0b011 << 5)			// 64SPS
#define ADS1115_DATA_RATE_128	(0b100 << 5)			// 128SPS
#define ADS1115_DATA_RATE_250	(0b101 << 5)			// 250SPS
#define ADS1115_DATA_RATE_475	(0b110 << 5)			// 475SPS
#define ADS1115_DATA_RATE_860	(0b111 << 5)			// 860SPS

#define ADS1115_COMP_MODE 	(0b0 << 4) // Default
#define ADS1115_COMP_POL 	(0b0 << 3) // Default
#define ADS1115_COMP_LAT 	(0b0 << 2) // Default
#define ADS1115_COMP_QUE 	(0b11)	   // Default

/* ADS1115 register configurations */
#define ADS1115_CONVER_REG 		0x00
#define ADS1115_CONFIG_REG 		0x01
#define ADS1115_LO_THRESH_REG	0x10
#define ADS1115_HI_THRESH_REG	0x11

#define ADS1115_ADDR			0X90
#define ADS1115_BUS_BUSY_LIMIT	100

typedef enum _ADS1115_read_state_t_
{
    ADS1115_STATE_RESET         = 0,    /* Entry: clear buffers                       */
    ADS1115_STATE_TRIGGER_CONV,         /* Write Config (OS=1, MUX, PGA, DR)          */
    ADS1115_STATE_WAIT_WRITE_DONE,      /* Wait for Config write I2C complete          */
    ADS1115_STATE_POLL_OS,              /* Read Config register, check OS bit          */
    ADS1115_STATE_FETCH_DATA,           /* OS=1 confirmed, read Conversion register    */
    ADS1115_STATE_PROCESS,              /* Wait read done, compensate, publish result  */
} ADS1115_read_state_t;

typedef struct _ADS1115_config_
{
	uint8_t			input_channel;
	uint8_t			pga;
	uint8_t			data_rate;

}ADS1115_config;

typedef struct _ADS1115_data_t_
{
    int16_t  raw_value;
    int32_t  voltage_mV;

} ADS1115_data_t;

typedef struct _ADS_1115_handle_
{
	ADS1115_config	config;
	ADS1115_data_t	data;

    uint8_t             state;         /* ADS1115_read_state_t current step    */
    uint8_t             busy_count;    /* bus-busy watchdog counter             */
    uint8_t             tx_buf[10];     /* I2C TX: [reg_ptr, Config_MSB, LSB]   */
    uint8_t             rx_buf[10];     /* I2C RX: [MSB, LSB]*/
    i2c_result_t        write_flag;    /* async flag for write transactions     */
    i2c_result_t        read_flag;     /* async flag for read  transactions     */
    i2c_result_t        result;        /* last read result (set by driver)      */

} ADS_1115_handle;

void ADS1115_Init_Handle(ADS_1115_handle* p_handle);
i2c_result_t ADS1115_Read_Value(i2c_stdio_typedef* p_i2c, ADS_1115_handle* p_handle);
bool ADS1115_Is_Read_Complete(ADS_1115_handle* p_handle);






#endif /* DEVICE_DRV_ADS1115_ADS1115_H_ */
