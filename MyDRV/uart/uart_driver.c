/*
 * uart.c
 *
 *  Created on: Feb 25, 2025
 *      Author: CAO HIEU
 */

#include "../uart/uart_driver.h"
#include "board.h"

#define MY_CORE_H_
#define MY_USART_H_
#include "stm32f4_header.h"

#include "stdbool.h"
#include <stdint.h>
#include <stdbool.h>

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef USE_NODMA_UART
// #define ATOMIC_BLOCK_START(uart) LL_USART_DisableIT_RXNE(uart)
// #define ATOMIC_BLOCK_END(uart)   LL_USART_EnableIT_RXNE(uart)




/*************************************************
 *                  HELPER                       *
 *************************************************/


/*************************************************
 *                    Init                       *
 *************************************************/

uint8_t UART_Driver_Init(UART_Driver_t* p_uart, char* name, USART_TypeDef* _handle, IRQn_Type _irqn,
		RingBufElement* _p_TX_buffer, RingBufElement* _p_RX_buffer, RingBufCtr _TX_size, RingBufCtr _RX_size )
{
	p_uart -> handle = _handle;
	p_uart -> irqn = _irqn;
	p_uart->name = name;

    RingBuffer_Create(&p_uart->rx_buffer, 1, name, _p_RX_buffer, _RX_size);
    RingBuffer_Create(&p_uart->tx_buffer, 2, name, _p_TX_buffer, _TX_size);

    if (_TX_size > 0)
    {
        memset((void *)_p_TX_buffer, 0, (size_t)_TX_size);
    }

    if (_RX_size > 0)
    {
        memset((void *)_p_RX_buffer, 0, (size_t)_RX_size);
    }

    LL_USART_DisableIT_TXE(p_uart->handle);
    LL_USART_EnableIT_RXNE(p_uart->handle);	// Start the RX IT, it will only run when there is a data send to the MCU.

    return 1;
}

/*************************************************
 *                     API                       *
 *************************************************/

int UART_Driver_Read(UART_Driver_t* p_uart)
{
    RingBufElement data;

    if (RingBuffer_Get(&p_uart->rx_buffer, &data))
    {
        return data;
    }
    return -1;
}

void UART_Driver_Write(UART_Driver_t* p_uart, uint8_t data)
{
    while (!RingBuffer_Put(&p_uart->tx_buffer, data))
    {

    }
    LL_USART_EnableIT_TXE(p_uart->handle);
}

void UART_Driver_SendString(UART_Driver_t* p_uart, const char *str)
{
    while (*str)
    {
        UART_Driver_Write(p_uart, (uint8_t)(*str));
        str++;
    }
}

void UART_Driver_SendFSP(UART_Driver_t* p_uart,const char *str, uint8_t len)
{
	uint8_t udx;

	for (udx = 0; udx < len; udx++)
	{
		UART_Driver_Write(p_uart, (uint8_t)(*str++));
	}

}

_Bool UART_Driver_IsDataAvailable(UART_Driver_t* p_uart)
{

    return RingBuffer_IsDataAvailable(&p_uart->rx_buffer);
}

uint16_t UART_Driver_TXNumFreeSlots(UART_Driver_t* p_uart)
{
    return (uint16_t)RingBuffer_NumFreeSlots(&p_uart->tx_buffer);
}

uint16_t UART_Driver_RXNumFreeSlots(UART_Driver_t* p_uart)
{

    return (uint16_t)RingBuffer_NumFreeSlots(&p_uart->rx_buffer);
}

void UART_Driver_FlushRx(UART_Driver_t* p_uart)
{
    memset(p_uart->rx_buffer.buffer, 0, p_uart->rx_buffer.max_size);
    atomic_store_explicit(&p_uart->rx_buffer.head, 0U, memory_order_release);
    atomic_store_explicit(&p_uart->rx_buffer.tail, 0U, memory_order_release);
}

void UART_Driver_FlushTx(UART_Driver_t* p_uart)
{
    memset(p_uart->tx_buffer.buffer, 0, p_uart->tx_buffer.max_size);
    atomic_store_explicit(&p_uart->tx_buffer.head, 0U, memory_order_release);
    atomic_store_explicit(&p_uart->tx_buffer.tail, 0U, memory_order_release);
}

void UART_Driver_Flush(UART_Driver_t* p_uart)
{
    UART_Driver_FlushRx(p_uart);
    UART_Driver_FlushTx(p_uart);
}

//void UART_Driver_ISR(UART_Driver_t* p_uart)
//{
//    if ((LL_USART_IsActiveFlag_RXNE(p_uart->handle) != RESET) &&
//        (LL_USART_IsEnabledIT_RXNE(p_uart->handle) != RESET))
//    {
//        uint8_t data = LL_USART_ReceiveData8(p_uart->handle);
//
//        if (LL_USART_IsActiveFlag_ORE(p_uart->handle))
//        {
//            LL_USART_ClearFlag_ORE(p_uart->handle);
//        }
//        if (LL_USART_IsActiveFlag_FE(p_uart->handle))
//        {
//            LL_USART_ClearFlag_FE(p_uart->handle);
//        }
//        if (LL_USART_IsActiveFlag_NE(p_uart->handle))
//        {
//            LL_USART_ClearFlag_NE(p_uart->handle);
//        }
//        RingBuffer_Put(&p_uart->rx_buffer, data);
//    }
//
//    if ((LL_USART_IsActiveFlag_TXE(p_uart->handle) != RESET) &&
//        (LL_USART_IsEnabledIT_TXE(p_uart->handle) != RESET))
//    {
//        uint8_t tx_data;
//        if (RingBuffer_Get(&p_uart->tx_buffer, &tx_data))
//        {
//            LL_USART_TransmitData8(p_uart->handle, tx_data);
//        }
//        else
//        {
//            LL_USART_DisableIT_TXE(p_uart->handle);
//        }
//    }
//}

void UART_Driver_ISR(UART_Driver_t* p_uart)
{
    // 1. XỬ LÝ CỜ LỖI ĐỘC LẬP (Đưa ra ngoài khối RXNE)
    if (LL_USART_IsActiveFlag_ORE(p_uart->handle))
    {
        LL_USART_ClearFlag_ORE(p_uart->handle);
    }
    if (LL_USART_IsActiveFlag_FE(p_uart->handle))
    {
        LL_USART_ClearFlag_FE(p_uart->handle);
    }
    if (LL_USART_IsActiveFlag_NE(p_uart->handle))
    {
        LL_USART_ClearFlag_NE(p_uart->handle);
    }

    // 2. XỬ LÝ NHẬN (RX)
    if ((LL_USART_IsActiveFlag_RXNE(p_uart->handle) != RESET) &&
        (LL_USART_IsEnabledIT_RXNE(p_uart->handle) != RESET))
    {
        uint8_t data = LL_USART_ReceiveData8(p_uart->handle);
        RingBuffer_Put(&p_uart->rx_buffer, data);
    }

    // 3. XỬ LÝ TRUYỀN (TX)
    if ((LL_USART_IsActiveFlag_TXE(p_uart->handle) != RESET) &&
        (LL_USART_IsEnabledIT_TXE(p_uart->handle) != RESET))
    {
        uint8_t tx_data;
        if (RingBuffer_Get(&p_uart->tx_buffer, &tx_data))
        {
            LL_USART_TransmitData8(p_uart->handle, tx_data);
        }
        else
        {
            // Tắt ngắt TXE khi đã gửi hết data trong buffer
            LL_USART_DisableIT_TXE(p_uart->handle);
        }
    }
}
#endif


