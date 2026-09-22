#ifndef __ADC_TEMP_H
#define __ADC_TEMP_H

#include "stm32f10x.h"

void ADC1_TempSensor_Init(void);
uint16_t ADC1_ReadRaw(void);
float ADC1_CalculateTemp(uint16_t adc_value);

#endif
