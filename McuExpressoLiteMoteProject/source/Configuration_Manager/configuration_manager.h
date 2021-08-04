/*
 * configuration_manager.h
 *
 *  Created on: 16 feb. 2019
 *      Author: MAX PC
 */

#ifndef CONFIGURATION_MANAGER_CONFIGURATION_MANAGER_H_
#define CONFIGURATION_MANAGER_CONFIGURATION_MANAGER_H_

#include "globals.h"

#include "packet_manager/packet_manager.h"

#define BACKDOOR_BASE_ADDR 0

//defining which position is taking the fist data byte when set in persistence
//TODO just address the rest of the data
#define CRC_FLASH_DATA_MSG_BYTES 2

#define MSG_IDENTITY_FLASH_OFFSET           0
#define MSG_HEALTH_CONF_FLASH_OFFSET        (MSG_IDENTITY_FLASH_OFFSET          + MSG_IDENTITY_SEND_LENGTH      + CRC_FLASH_DATA_MSG_BYTES)
#define MSG_ADC_CALIBRATION_FLASH_OFFSET    (MSG_HEALTH_CONF_FLASH_OFFSET       + MSG_HEALTH_CONF_SEND_LENGTH   + CRC_FLASH_DATA_MSG_BYTES)
#define MSG_RADIO_CONFIG_FLASH_OFFSET       (MSG_ADC_CALIBRATION_FLASH_OFFSET   + MSG_ADC_CAL_SEND_LENGTH       + CRC_FLASH_DATA_MSG_BYTES)

//defines for updating node configuration parameters
#define IDENTITY_CONFIG_NODE_DATA   0
#define HEALTH_CONFIG_NODE_DATA     1
#define ADC_CONFIG_CALIBRATION_DATA 2
#define RADIO_CONFIG_NODE_DATA      3

void CnfManager_Init(void);

UINT8 CnfManager_Load_Mote_Default_Config(void);

UINT16 CnfManager_Get_My_Address(void);
UINT16 CnfManager_Get_Health_Msg_Period(void);

void CnfManager_Save_Data(void *dataBuffer, UINT16 msgType);
UINT8 CnfManager_Load_Data(void *dataBuffer, UINT16 msgType);

UINT8 CnfManager_Update_Node_Config(UINT8 dataConfigToBeUpdated);

void Radio_Interface_Load_Parameters(TMote_Radio_Conf_Data current_Radio_Conf_Data);

#endif /* CONFIGURATION_MANAGER_CONFIGURATION_MANAGER_H_ */
