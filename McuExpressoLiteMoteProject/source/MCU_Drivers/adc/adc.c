/*
 * adc.c
 *
 *  Created on: 13 may. 2020
 *      Author: MAX PC
 */

#include <MCU_Drivers/adc/adc.h>
#include "MKL82Z7.h"

#include "fsl_clock.h"

UINT8 adcInitialized = FALSE;

//*****************************************************************************
void ADC_Pinout_Init(void)
//*****************************************************************************
// Inits ADC pinout
//*****************************************************************************
{

}

//*****************************************************************************
void ADC_DeInit(void)
//*****************************************************************************
// DeInits the ADC
//*****************************************************************************
{
  //disable pmc bandgap
  PMC->REGSC &= 0xEF;

  ADC_Disable();

  adcInitialized = FALSE;
}

//*****************************************************************************
void ADC_Init(void)
//*****************************************************************************
// Inits the ADC channels
//*****************************************************************************
{
  //configure ADC0
  CLOCK_EnableClock(kCLOCK_Adc0);

  //no interrupt triggered
  //single ended mode
  //input channel (disabled)
  //ADC0->SC1[0] = 0x1F;

  //low power, yes
  //clock input divider 8
  //short sample time
  //16 bit conversion
  //bus clock selected
  ADC0->CFG1 = 0xFC;

  //Adxxa channels selected
  //asyncrhonous clock enabled
  //normal conversion sequence selected
  //default longest sample time
  ADC0->CFG2 = 0x00;

  //software trigger selected
  //compare function disabled
  //less than threshold
  //range function disabled
  //dma disabled
  //deafult vrefh vrefl selected
  ADC0->SC2 = 0;

  //hw average activated
  //32 samples to average the result
  //ADC0->SC3 = 0x07;
  ADC0->SC3 = 0x07;

  adcInitialized = TRUE;
}

//*****************************************************************************
UINT8 ADC_AutoCalibration(void)
//*****************************************************************************
// Calibrates the ADC, ADC peripheral must be initialized prior to calibration
//*****************************************************************************
{
  UINT8 retVal = OK;
  volatile UINT32 tmp32 = 0;

  if(adcInitialized == FALSE)
  {
    ADC_Init();
  }

  //enable pmc bandgap
  PMC->REGSC |= 0x05;

  //clean CALF bit
  ADC0->SC3 &= ADC_SC3_CALF_MASK;

  //start calibration
  ADC0->SC3 |= 0x80;

  //wait for CAL bit to be cleared
  while( (ADC0->SC3 & 0x80) == 0x80 )
  {
    if( (ADC0->SC3 & 0x40) == 0x40)
    {
      //calibration failed
      retVal = FAILED;
      break;
    }
  }

  tmp32 = ADC0->R[0];

  tmp32 = ADC0->OFS;

  if(retVal != FAILED)
  {
    //fill the calibration registers
    tmp32 = ADC0->CLP0 + ADC0->CLP1 + ADC0->CLP2 + ADC0->CLP3 + ADC0->CLP4 + ADC0->CLPS;
    tmp32 = 0x8000U | (tmp32 >> 1U);
    ADC0->PG = tmp32;

    //for differential modes
    tmp32 = ADC0->CLM0 + ADC0->CLM1 + ADC0->CLM2 + ADC0->CLM3 + ADC0->CLM4 + ADC0->CLMS;
    tmp32 = 0x8000U | (tmp32 >> 1U);
    ADC0->MG = tmp32;
  }

  //disable pmc bandgap
  PMC->REGSC &= 0x04;

  return retVal;
}

//*****************************************************************************
void ADC_Set_Calibration_Manually(UINT16 mSide, UINT16 pSide, UINT16 offSet)
//*****************************************************************************
// Sets the calibrations values
//*****************************************************************************
{
  ADC0->OFS = (UINT32)(offSet & 0x0000FFFF);
  ADC0->PG  = (UINT32)(pSide & 0x0000FFFF);
  ADC0->MG  = (UINT32)(mSide & 0x0000FFFF);
}

//*****************************************************************************
void ADC_Get_Calibration_Manually(UINT8 *data)
//*****************************************************************************
// returns the calibrations values in a buffer
//*****************************************************************************
{

  data[0] = (UINT8)((ADC0->OFS & 0x0000FF00) >> 8);
  data[1] = (UINT8)((ADC0->OFS & 0x000000FF));

  data[2] = (UINT8)((ADC0->PG & 0x0000FF00) >> 8);
  data[3] = (UINT8)((ADC0->PG & 0x000000FF));


  data[4] = (UINT16)(ADC0->MG & 0x0000FFFF);
  data[5] = (UINT16)(ADC0->MG & 0x0000FFFF);
}

//*****************************************************************************
void ADC_StartConversion(UINT8 channel)
//*****************************************************************************
// starts a converion using current configuration and calibration
//*****************************************************************************
{
  //config the channel
  //disabled ADC
  //select channel to convert
  ADC0->SC1[0] |= 0x1F;
  ADC0->SC1[0] = (UINT32)(channel & 0x1F);
}

//*****************************************************************************
void ADC_Disable(void)
//*****************************************************************************
// disable adc module
//*****************************************************************************
{
  //disable adc
  ADC0->SC1[0] |= 0x1F;
}

//*****************************************************************************
UINT8 ADC_WaitForConversion(void)
//*****************************************************************************
// Check if adc is busy
//*****************************************************************************
{
  UINT8 retVal = BUSY;

  //wait for conversion
  if((ADC0->SC1[0] & 0x00000080) == 0x80)
  {
    retVal = NOT_BUSY;
  }
  else
  {
    retVal = BUSY;
  }

  return retVal;
}

//*****************************************************************************
UINT16 ADC_GetData(void)
//*****************************************************************************
// Starts a conversion on the selected channel and returns the data converted
// working in blocking mode (no need to isr mode, not much data is converted)
//*****************************************************************************
{
  UINT32 retValue=0;

  retValue = (ADC0->R[0] & 0x0000FFFF);

  return retValue;
}

//*****************************************************************************
void ADC_Test(void)
//*****************************************************************************
// Testing adc
//*****************************************************************************
{
  UINT32 tempADCValue=0;
  float temp=0;
  UINT32 vref=0;

  //avoid not used warnings at compile
  vref = temp;
  temp = vref;

  ADC_Init();
  ADC_AutoCalibration(); //should be made at 25 Cº

  ADC_StartConversion(TEMP_ADC_CHANNEL);
  while(ADC_WaitForConversion());
  ADC_Disable();
  tempADCValue = ADC_GetData();

  temp = 25 - (((tempADCValue * 0.05035) - 716 )/(1.620));

  ADC_StartConversion(VREF_ADC_CHANNEL);
  while(ADC_WaitForConversion());
  vref = ADC_GetData();
  ADC_Disable();

  ADC_DeInit();
}

