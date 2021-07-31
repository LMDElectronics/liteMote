/*
 * adc.h
 *
 *  Created on: 13 may. 2020
 *      Author: MAX PC
 */

#ifndef HEALTH_MANAGER_ADC_ADC_H_
#define HEALTH_MANAGER_ADC_ADC_H_

#include "globals.h"


#define TEMP_ADC_CHANNEL 26
#define VREF_ADC_CHANNEL 29
#define BAND_GAP_CHANNEL 27

#define STANDARD_TEMP   25U
#define VREF_V          3.3

void ADC_Init(void);
void ADC_DeInit(void);

UINT8 ADC_AutoCalibration(void);

void ADC_Set_Calibration_Manually(UINT16 mSide, UINT16 pSide, UINT16 offSet);
void ADC_Get_Calibration_Manually(UINT8 *data);

void ADC_StartConversion(UINT8 channel);
void ADC_Disable(void);

UINT8 ADC_WaitForConversion(void);
UINT16 ADC_GetData(void);

void ADC_Test(void);

#endif /* HEALTH_MANAGER_ADC_ADC_H_ */
