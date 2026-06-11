/*
 * spi.h
 *
 *  Created on: May 4, 2026
 *      Author: PV
 */

#ifndef SPI_SPI_H_
#define SPI_SPI_H_


#define MY_CORE_H_
#define MY_SPI_H_
#include "stm32f4_header.h"

#include "stdbool.h"

typedef enum {
    SPI_STATE_READY = 0,
    SPI_STATE_BUSY_TX,
    SPI_STATE_BUSY_RX,
    SPI_STATE_BUSY_TX_RX
} SPI_DMA_state;


typedef struct _spi_stdio_typedef
{
	SPI_TypeDef*		spi_handle;

	GPIO_TypeDef*		cs_port;
	uint32_t			cs_pin;

	DMA_TypeDef*		dma_handle;
    uint32_t 			dma_stream_tx;         // Stream TX (Stream 5)
    uint32_t 			dma_stream_rx;         // Stream RX (Stream 6)
    uint32_t 			dma_channel;

    uint8_t*			tx_buffer;
    uint8_t*			rx_buffer;

    bool				transfer_done;
    SPI_DMA_state		spi_state;

} SPI_DMA_Driver_t;

void SPI_DMA_InitHandle(SPI_DMA_Driver_t* p_spi);
void SPI_DMA_Transfer(SPI_DMA_Driver_t* p_spi, uint32_t size);



#endif /* SPI_SPI_H_ */
