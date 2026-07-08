/*
 * TC1047AVNBTR.h
 *
 *  Created on: Jul 7, 2026
 *      Author: PV
 */

#ifndef DEVICE_DRV_TC1047AVNBTR_TC1047_H_
#define DEVICE_DRV_TC1047AVNBTR_TC1047_H_

#include "stm32f4xx_ll_adc.h"
#include <stdint.h>

#define ADC_POLLING_TIMEOUT 100000U


typedef struct {
    ADC_TypeDef *	adc_handle;
    uint32_t 		adc_channel;
    float 			vref_mv;
    float 			temp_value;
} TC1047_Handle_t;


void TC1047_Init(TC1047_Handle_t *htc, ADC_TypeDef *ADCx, uint32_t Channel, float Vref_mV);

uint16_t TC1047_ReadRaw(TC1047_Handle_t *htc);

float TC1047_GetTemperature(TC1047_Handle_t *htc);

#endif /* DEVICE_DRV_TC1047AVNBTR_TC1047_H_ */
