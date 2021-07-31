/*
 * packet_adc_cal_val.h
 *
 *  Created on: 14 may. 2020
 *      Author: MAX PC
 */

#ifndef PACKET_MANAGER_PACKET_ADC_CALIBRATION_PACKET_ADC_CAL_VAL_H_
#define PACKET_MANAGER_PACKET_ADC_CALIBRATION_PACKET_ADC_CAL_VAL_H_

#define MSG_ADC_CAL_SEND_LENGTH             sizeof(TMote_ADC_Cal_Data)
#define MSG_ADC_CALHEALTH_RETRIEVE_LENGTH   0

typedef struct __attribute__((packed)) TMote_ADC_Cal_Data
{
  //just for kl82 kinetis MCU calibration values
  UINT16 offsetCorrection;
  UINT16 pSideGain;
  UINT16 mSideGain;

}TMote_ADC_Cal_Data;

#endif /* PACKET_MANAGER_PACKET_ADC_CALIBRATION_PACKET_ADC_CAL_VAL_H_ */
