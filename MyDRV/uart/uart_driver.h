/*
 * uart.h
 *
 *  Created on: Feb 25, 2025
 *      Author: CAO HIEU
 */

#define USE_NODMA_UART
#ifdef USE_NODMA_UART

#ifndef M1_DRIVERS_UART_UART_DRIVER_H_
#define M1_DRIVERS_UART_UART_DRIVER_H_

#define MY_CORE_H_
#include "stm32f4_header.h"

#include "../uart/ring_buffer.h"
#include <stdint.h>

#define UART6_BUFFER_SIZE    1024

typedef struct {

    USART_TypeDef* 		handle;
    IRQn_Type			irqn;
    char*				name;
    s_RingBufferType rx_buffer;  // Ringbuffer Rx
    s_RingBufferType tx_buffer;  // Ringbuffer Tx

} UART_Driver_t;

uint8_t UART_Driver_Init(UART_Driver_t* p_uart, char* name, USART_TypeDef* _handle, IRQn_Type _irqn,
		RingBufElement* _p_TX_buffer, RingBufElement* _p_RX_buffer, RingBufCtr _TX_size, RingBufCtr _RX_size);
void UART_Driver_ISR(UART_Driver_t* p_uart);
int  UART_Driver_Read(UART_Driver_t* p_uart);
void UART_Driver_Write(UART_Driver_t* p_uart, uint8_t data);
void UART_Driver_SendString(UART_Driver_t* p_uart, const char *str);
void UART_Driver_SendFSP(UART_Driver_t* p_uart,const char *str, uint8_t len);
_Bool UART_Driver_IsDataAvailable(UART_Driver_t* p_uart);
uint16_t UART_Driver_TXNumFreeSlots(UART_Driver_t* p_uart);
uint16_t UART_Driver_RXNumFreeSlots(UART_Driver_t* p_uart);
// --- API: Flush Rx/Tx or Both ---
void UART_Driver_FlushRx(UART_Driver_t* p_uart);
void UART_Driver_FlushTx(UART_Driver_t* p_uart);
void UART_Driver_Flush(UART_Driver_t* p_uart);

#endif

#endif /* M1_DRIVERS_UART_UART_DRIVER_H_ */
