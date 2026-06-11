

#include "spi.h"

#include <stdint.h>

#define MY_CORE_H_
#define MY_SPI_H_
#define MY_DMA_H_
#define MY_GPIO_H_

#include "stm32f4_header.h"



void SPI_DMA_InitHandle(SPI_DMA_Driver_t* p_spi){

	LL_DMA_DisableStream(p_spi->dma_handle, p_spi->dma_stream_tx);
	LL_DMA_DisableStream(p_spi->dma_handle, p_spi->dma_stream_rx);

	LL_DMA_SetMode(p_spi->dma_handle, p_spi->dma_stream_tx, LL_DMA_MODE_NORMAL);
	LL_DMA_SetMode(p_spi->dma_handle, p_spi->dma_stream_rx, LL_DMA_MODE_NORMAL);

	LL_DMA_SetPeriphAddress(p_spi->dma_handle, p_spi->dma_stream_tx, (uint32_t)&(p_spi->spi_handle->DR));
	LL_DMA_SetPeriphAddress(p_spi->dma_handle, p_spi->dma_stream_rx, (uint32_t)&(p_spi->spi_handle->DR));

	if (LL_DMA_IsActiveFlag_TC0(p_spi->dma_handle))
	{
		LL_DMA_ClearFlag_TC0(p_spi->dma_handle);
	}
	if (LL_DMA_IsActiveFlag_TC5(p_spi->dma_handle))
	{
		LL_DMA_ClearFlag_TC5(p_spi->dma_handle);
	}

	LL_DMA_EnableIT_TC(p_spi->dma_handle, p_spi->dma_stream_rx);

}



void SPI_DMA_Transfer(SPI_DMA_Driver_t* p_spi, uint32_t size){

	LL_DMA_DisableStream(p_spi->dma_handle, p_spi->dma_stream_tx);
	LL_DMA_DisableStream(p_spi->dma_handle, p_spi->dma_stream_rx);

	LL_DMA_SetDataLength(p_spi->dma_handle, p_spi->dma_stream_tx, size);
	LL_DMA_SetDataLength(p_spi->dma_handle, p_spi->dma_stream_rx, size);

	//Stream TX
	LL_DMA_SetMemoryAddress(p_spi->dma_handle, p_spi->dma_stream_tx, (uint32_t)p_spi->tx_buffer);
	LL_DMA_SetMemoryAddress(p_spi->dma_handle, p_spi->dma_stream_rx, (uint32_t)p_spi->rx_buffer);


	if (LL_SPI_IsActiveFlag_RXNE(p_spi->spi_handle)) {
	        LL_SPI_ReceiveData8(p_spi->spi_handle);
	    }

	LL_SPI_EnableDMAReq_TX(p_spi->spi_handle);
	LL_SPI_EnableDMAReq_RX(p_spi->spi_handle);

	LL_DMA_EnableStream(p_spi->dma_handle, p_spi->dma_stream_rx);
	LL_DMA_EnableStream(p_spi->dma_handle, p_spi->dma_stream_tx);
}
/**
 * @brief  Hàm xử lý ngắt khi DMA TX hoàn thành.
 * @note   Hàm này CẦN được gọi bên trong DMAx_StreamY_IRQHandler của file it.c
 */
void SPI_DMA_IRQHandler_RX(SPI_DMA_Driver_t* p_spi)
{
    /* Kiểm tra xem SPI đã hết Busy chưa trước khi xử lý (Rất quan trọng với luồng Master) */
    if (!LL_SPI_IsActiveFlag_BSY(p_spi->spi_handle)) {

        /* 1. Kéo chân CS lên để ngắt kết nối với FRAM */
        LL_GPIO_SetOutputPin(p_spi->cs_port, p_spi->cs_pin);

        /* 2. Dọn dẹp cờ Request */
        LL_SPI_DisableDMAReq_TX(p_spi->spi_handle);
        LL_SPI_DisableDMAReq_RX(p_spi->spi_handle);

        /* 3. Đánh dấu trạng thái rảnh rỗi */
        p_spi->spi_state = SPI_STATE_READY;
        p_spi->transfer_done = true;

        /* Chú ý: Nếu đây là quá trình hàm Read, đây là lúc bạn copy dữ liệu từ fram_rx_buffer[4]
           trả về cho Application. Bạn có thể dùng Event Group / Flag của FreeRTOS tại đây */
    }
}

