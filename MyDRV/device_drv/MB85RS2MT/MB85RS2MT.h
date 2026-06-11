/*
 * MB85RS2MT.h
 *
 *  Created on: May 4, 2026
 *      Author: PV
 */

#ifndef DEVICE_DRV_MB85RS2MT_MB85RS2MT_H_
#define DEVICE_DRV_MB85RS2MT_MB85RS2MT_H_

#define MY_CORE_H_
#define MY_SPI_H_
#define MY_DMA_H_
#include "stm32f4_header.h"

#include "board.h"
#include "spi.h"

/* -------- Commands -------- */

#define MB85RS2MT_CMD_WREN     0x06  /* Enable Write */
#define MB85RS2MT_CMD_WRDI     0x04  /* Disable Write */
#define MB85RS2MT_CMD_RDSR     0x05  /* Read Status */
#define MB85RS2MT_CMD_WRSR     0x01  /* Write Status */
#define MB85RS2MT_CMD_READ     0x03  /* Read Data */
#define MB85RS2MT_CMD_WRITE    0x02  /* Write Data */
#define MB85RS2MT_CMD_SLEEP    0xB9  /* Sleep Mode */
#define MB85RS2MT_CMD_RDID     0xAB  /* Wake Up */

void mb85rs2mt_init(void);

void mb85rs2mt_write(uint32_t address, uint8_t *buffer, uint16_t length);

void mb85rs2mt_read(uint32_t address, uint8_t *buffer, uint16_t length);



#endif /* DEVICE_DRV_MB85RS2MT_MB85RS2MT_H_ */
