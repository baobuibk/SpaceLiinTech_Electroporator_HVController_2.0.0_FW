/*
 * MB85RS2MT.c
 *
 *  Created on: May 4, 2026
 *      Author: PV
 */

#include "mb85rs2mt.h"

#include <string.h>
#include <stdio.h>

#define FRAM_BUFFER_SIZE     (1024 + 4)

static uint8_t fram_tx_buffer[FRAM_BUFFER_SIZE];
static uint8_t fram_rx_buffer[FRAM_BUFFER_SIZE];


SPI_DMA_Driver_t mb85rs2mt_dev = {
	.spi_handle = FRAM_SPI_HANDLE,
	.cs_port	= FRAM_SPI_CS_PORT,
	.cs_pin		= FRAM_SPI_CS_PIN,
	.dma_handle = FRAM_DMA_HANDLE,
	.dma_channel= FRAM_DMA_CHANNEL,
	.dma_stream_tx = FRAM_DMA_TX_STREAM,
	.dma_stream_rx = FRAM_DMA_RX_STREAM,
	.spi_state	= SPI_STATE_READY,
	.transfer_done = false
};

static inline void mb85rs2mt_select(void) {
    LL_GPIO_ResetOutputPin(mb85rs2mt_dev.cs_port, mb85rs2mt_dev.cs_pin);
}

static inline void mb85rs2mt_deselect(void) {
    LL_GPIO_SetOutputPin(mb85rs2mt_dev.cs_port, mb85rs2mt_dev.cs_pin);
}

void mb85rs2mt_init(void){
    SPI_DMA_InitHandle(&mb85rs2mt_dev);
    mb85rs2mt_deselect();
}


static void mb85rs2mt_write_enable(void) {
    uint8_t cmd = MB85RS2MT_CMD_WREN;

    mb85rs2mt_select();

    LL_SPI_TransmitData8(mb85rs2mt_dev.spi_handle, cmd);

    while (!LL_SPI_IsActiveFlag_TXE(mb85rs2mt_dev.spi_handle));
    while (LL_SPI_IsActiveFlag_BSY(mb85rs2mt_dev.spi_handle));
    LL_SPI_ReceiveData8(mb85rs2mt_dev.spi_handle);

    mb85rs2mt_deselect();
}

void mb85rs2mt_write(uint32_t address, uint8_t *buffer, uint16_t length) {
    if (!buffer || length == 0 || length > 1024) return;


    if (mb85rs2mt_dev.spi_state != SPI_STATE_READY) return;

    mb85rs2mt_write_enable();

    uint32_t total_length = length + 4;


    fram_tx_buffer[0] = MB85RS2MT_CMD_WRITE;
    fram_tx_buffer[1] = (uint8_t) (address >> 16);
    fram_tx_buffer[2] = (uint8_t) (address >> 8);
    fram_tx_buffer[3] = (uint8_t) (address);

    memcpy(&fram_tx_buffer[4], buffer, length);

    mb85rs2mt_dev.tx_buffer = fram_tx_buffer;
    mb85rs2mt_dev.rx_buffer = fram_rx_buffer;

    mb85rs2mt_dev.spi_state = SPI_STATE_BUSY_TX_RX;
    mb85rs2mt_dev.transfer_done = false;

    mb85rs2mt_select();
    SPI_DMA_Transfer(&mb85rs2mt_dev, total_length);

}

void mb85rs2mt_read(uint32_t address, uint8_t *buffer, uint16_t length) {
    if (!buffer || length == 0 || length > 1024) return;
    if (mb85rs2mt_dev.spi_state != SPI_STATE_READY) return;

    uint32_t total_length = length + 4;

    fram_tx_buffer[0] = MB85RS2MT_CMD_READ;
    fram_tx_buffer[1] = (uint8_t) (address >> 16);
    fram_tx_buffer[2] = (uint8_t) (address >> 8);
    fram_tx_buffer[3] = (uint8_t) (address);

    memset(&fram_tx_buffer[4], 0, length);

    mb85rs2mt_dev.tx_buffer = fram_tx_buffer;
    mb85rs2mt_dev.rx_buffer = fram_rx_buffer;

    mb85rs2mt_dev.spi_state = SPI_STATE_BUSY_TX_RX;
    mb85rs2mt_dev.transfer_done = false;

    mb85rs2mt_select();
    SPI_DMA_Transfer(&mb85rs2mt_dev, total_length);
}





