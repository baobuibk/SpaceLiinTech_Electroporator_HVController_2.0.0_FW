/*
 * board.h
 *
 *  Created on: Apr 22, 2026
 *      Author: PV
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "stm32f4xx_ll_gpio.h"

/*********************ADC***********************/
#define ADC_FEEDBACK_HANDLE     ADC1
#define ADC_FEEDBACK_IRQ        ADC1_IRQn

#define ADC_CHANNEL_COUNT       2
#define ADC_300V_CHANNEL        LL_ADC_CHANNEL_12
#define ADC_50V_CHANNEL         LL_ADC_CHANNEL_10

#define ADC_TEMP_HANDLE			ADC2

#define ADC_TEMP_CHANNEL_COUNT 	2
#define ADC_TEMP_300V_CHANNEL	LL_ADC_CHANNEL_14
#define ADC_TEMP_50V_CHANNEL  	LL_ADC_CHANNEL_15
/***********************************************/

/*********************FLYBACK*******************/
#define FLYBACK_SW1_HANDLE      TIM1
#define FLYBACK_SW1_CHANNEL     LL_TIM_CHANNEL_CH4

#define FLYBACK_SD1_PORT        GPIOA
#define FLYBACK_SD1_PIN         LL_GPIO_PIN_9

#define DISCHARGE_300V_PORT     GPIOA
#define DISCHARGE_300V_PIN      LL_GPIO_PIN_10


#define FLYBACK_SW2_HANDLE      TIM3
#define FLYBACK_SW2_CHANNEL     LL_TIM_CHANNEL_CH2

#define FLYBACK_SD2_PORT        GPIOD
#define FLYBACK_SD2_PIN         LL_GPIO_PIN_2

#define DISCHARGE_50V_PORT      GPIOB
#define DISCHARGE_50V_PIN       LL_GPIO_PIN_4

#define OVP_PORT            	 GPIOC
#define OVP_300_PIN             LL_GPIO_PIN_3
#define OVP_300_EXTI			LL_EXTI_LINE_3

#define OVP_50_PIN              LL_GPIO_PIN_1
#define OVP_50_EXTI				LL_EXTI_LINE_1
/***********************************************/

/*********************UART**********************/
#define DEBUG_UART_HANDLE       USART6
#define DEBUG_UART_IRQ          USART6_IRQn

#define GPP_UART_HANDLE       	USART1
#define GPP_UART_IRQ          	USART1_IRQn

#define GPP_TX_SIZE			    5120
#define	GPP_RX_SIZE			    5120
#define FSP_BUF_LEN				1024

/*********************SPI***********************/
// #define FRAM_SPI_HANDLE         SPI3

// #define FRAM_SPI_CS_PORT        GPIOA
// #define FRAM_SPI_CS_PIN         LL_GPIO_PIN_12

// #define FRAM_DMA_HANDLE			DMA1
// #define FRAM_DMA_CHANNEL		LL_DMA_CHANNEL_0
// #define FRAM_DMA_TX_STREAM		LL_DMA_STREAM_5
// #define FRAM_DMA_RX_STREAM		LL_DMA_STREAM_5

/**********************I2C**********************/

#define ONBOARD_SENSOR_I2C_HANDLE   I2C1
#define ONBOARD_SENSOR_I2C_IRQ      I2C1_EV_IRQn

#define CHARGER_I2C_HANDLE   		I2C2
#define CHARGER_SENSOR_I2C_IRQ      I2C2_EV_IRQn

#define SENSOR_I2C_HANDLE           I2C3
#define SENSOR_I2C_IRQ              I2C3_EV_IRQn

#define SENSOR_INT_PORT             GPIOB

#define SENSOR_H3LIS331DL_INT1_PIN            LL_GPIO_PIN_2
#define SENSOR_H3LIS331DL_INT2_PIN            LL_GPIO_PIN_0
#define SENSOR_LSM6DSOX_INT1_PIN              LL_GPIO_PIN_1
#define SENSOR_LSM6DSOX_INT2_PIN              LL_GPIO_PIN_3







#endif /* BOARD_H_ */
