/*
 * TC1047AVNBTR.c
 *
 *  Created on: Jul 7, 2026
 *      Author: PV
 */

#include "TC1047.h"


void TC1047_Init(TC1047_Handle_t *htc, ADC_TypeDef *ADCx, uint32_t Channel, float Vref_mV) {
//    if (htc != NULL) {
        htc->adc_handle = ADCx;
        htc->adc_channel = Channel;
        htc->vref_mv = Vref_mV;
//    }
}

uint16_t TC1047_ReadRaw(TC1047_Handle_t *htc) {
    uint32_t timeout = ADC_POLLING_TIMEOUT;

    if (LL_ADC_IsEnabled(htc->adc_handle) == 0) {
        LL_ADC_Enable(htc->adc_handle);
    }

    LL_ADC_REG_SetSequencerRanks(htc->adc_handle, LL_ADC_REG_RANK_1, htc->adc_channel);

    LL_ADC_REG_StartConversionSWStart(htc->adc_handle);

    while (!LL_ADC_IsActiveFlag_EOCS(htc->adc_handle)) {
        timeout--;
        if (timeout == 0) {

            return 0;
        }
    }

    LL_ADC_ClearFlag_EOCS(htc->adc_handle);

    return LL_ADC_REG_ReadConversionData12(htc->adc_handle);
}

float TC1047_GetTemperature(TC1047_Handle_t *htc) {
    uint16_t raw_adc = TC1047_ReadRaw(htc);

    float voltage_mv = ((float)raw_adc * htc->vref_mv) / 4095.0f;

    float temperature_c = (voltage_mv - 500.0f) / 10.0f;

    return temperature_c;
}
