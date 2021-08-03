/*
 * configuration_manager.c
 *
 *  Created on: 16 feb. 2019
 *      Author: MAX PC
 */

#include <FlashManager/FlashDataRWInterface/FlashDataRWInterface.h>
#include "Configuration_Manager/configuration_manager.h"
#include "Health_Manager/health_manager.h"

//current config data that node will use
volatile TMote_Identity_Data    current_Identity_Config;
volatile TMote_Health_Conf_Data current_Health_Config;
volatile TMote_ADC_Cal_Data     current_ADC_Cal_Data;
volatile TMote_Radio_Conf_Data  current_Radio_Conf_Data;

//*****************************************************************************
void CnfManager_Init(void)
//*****************************************************************************
// inits the configuration manager
//*****************************************************************************
{

}

//*****************************************************************************
UINT8 CnfManager_Load_Mote_Identity_Config(void)
//*****************************************************************************
// Loads identity config persistent data
//*****************************************************************************
{
  UINT8 dataRead[MAX_PAYLOAD_BYTES];

  UINT8 dataOffset      = MSG_IDENTITY_FLASH_OFFSET;
  UINT8 numBytesToRead  = MSG_IDENTITY_SEND_LENGTH;
  UINT16 flashPage      = FLASH_PAGE_SYSTEM_DATA;

  UINT8 retVal=FAILED;

  if(FlashInterface_Load_SystemData_FromFlash(dataRead, flashPage, dataOffset, numBytesToRead) == OK)
  {
    current_Identity_Config.NodeID = (UINT16)dataRead[0];
    current_Identity_Config.NodeID <<=8;
    current_Identity_Config.NodeID |= (UINT16)dataRead[1];

    current_Identity_Config.NodeType = (UINT16)dataRead[2];
    current_Identity_Config.NodeType <<=8;
    current_Identity_Config.NodeType |= (UINT16)dataRead[3];

    current_Identity_Config.fw_version = (UINT16)dataRead[4];
    current_Identity_Config.fw_version <<=8;
    current_Identity_Config.fw_version |= (UINT16)dataRead[5];

    current_Identity_Config.fw_CRC = (UINT16)dataRead[6];
    current_Identity_Config.fw_CRC <<=8;
    current_Identity_Config.fw_CRC |= (UINT16)dataRead[7];

    current_Identity_Config.hw_version = (UINT16)dataRead[8];
    current_Identity_Config.hw_version <<=8;
    current_Identity_Config.hw_version |= (UINT16)dataRead[9];

    current_Identity_Config.hw_case = (UINT16)dataRead[10];
    current_Identity_Config.hw_case <<=8;
    current_Identity_Config.hw_case |= (UINT16)dataRead[11];

    retVal = OK;
  }
  else
  {
    //load data from flash, load mote default parameters if flash data load fails
    current_Identity_Config.NodeID        = 1;
    current_Identity_Config.NodeType      = 0;
    current_Identity_Config.fw_version    = 11;
    current_Identity_Config.fw_CRC        = 12345;
    current_Identity_Config.hw_version    = 15;
    current_Identity_Config.hw_case       = 11;

    retVal = FAILED;
  }

  return retVal;
}

//*****************************************************************************
UINT8 Cnf_Manager_Load_Mote_Health_Config(void)
//*****************************************************************************
// Loads health config persistent data
//*****************************************************************************
{
  UINT8 dataRead[MAX_PAYLOAD_BYTES];

  UINT8 dataOffset      = MSG_HEALTH_CONF_FLASH_OFFSET;
  UINT8 numBytesToRead  = MSG_HEALTH_CONF_SEND_LENGTH;
  UINT16 flashPage      = FLASH_PAGE_SYSTEM_DATA;

  UINT8 retVal=FAILED;

  if(FlashInterface_Load_SystemData_FromFlash(dataRead, flashPage, dataOffset, numBytesToRead) == OK)
  {
    current_Health_Config.healthPeriod = (UINT32)dataRead[0];
    current_Health_Config.healthPeriod <<=8;
    current_Health_Config.healthPeriod |= (UINT32)dataRead[1];

    retVal = OK;
  }
  else
  {
    //load data from flash, load mote default parameters if flash data load fails
    current_Health_Config.healthPeriod = 60; //every hour

    retVal = FAILED;
  }

  Health_Manager_Reload_Timer(current_Health_Config.healthPeriod);

  return retVal;
}

//*****************************************************************************
UINT8 Cnf_Manager_Load_ADC_Calibration_Config(void)
//*****************************************************************************
// Loads health config persistent data
//*****************************************************************************
{
  UINT8 dataRead[MAX_PAYLOAD_BYTES];

  UINT8 dataOffset      = MSG_ADC_CALIBRATION_FLASH_OFFSET;
  UINT8 numBytesToRead  = MSG_ADC_CAL_SEND_LENGTH;
  UINT16 flashPage      = FLASH_PAGE_SYSTEM_DATA;

  UINT8 retVal=FAILED;

  if(FlashInterface_Load_SystemData_FromFlash(dataRead, flashPage, dataOffset, numBytesToRead) == OK)
  {
    //load data from flash
    current_ADC_Cal_Data.offsetCorrection = (UINT16)dataRead[0];
    current_ADC_Cal_Data.offsetCorrection <<= 8;
    current_ADC_Cal_Data.offsetCorrection |= (UINT16)dataRead[1];

    current_ADC_Cal_Data.pSideGain = (UINT16)dataRead[2];
    current_ADC_Cal_Data.pSideGain <<= 8;
    current_ADC_Cal_Data.pSideGain |= (UINT16)dataRead[3];

    current_ADC_Cal_Data.mSideGain = (UINT16)dataRead[4];
    current_ADC_Cal_Data.mSideGain <<= 8;
    current_ADC_Cal_Data.mSideGain |= (UINT16)dataRead[5];

    retVal = OK;
  }
  else
  {
    //load mote default parameters if flash data load fails
    current_ADC_Cal_Data.offsetCorrection = 0x40;
    current_ADC_Cal_Data.pSideGain = 0x8200;
    current_ADC_Cal_Data.mSideGain = 0x8200;

    retVal = FAILED;
  }

  ADC_Init();
  ADC_Set_Calibration_Manually(current_ADC_Cal_Data.mSideGain, current_ADC_Cal_Data.pSideGain, current_ADC_Cal_Data.offsetCorrection);
  ADC_DeInit();

  return retVal;
}

//*****************************************************************************
UINT8 Cnf_Manager_Load_Radio_Config(void)
//*****************************************************************************
// Loads health config persistent data
//*****************************************************************************
{
  UINT8 dataRead[MAX_PAYLOAD_BYTES];

  UINT8 dataOffset      = MSG_RADIO_CONFIG_FLASH_OFFSET;
  UINT8 numBytesToRead  = MSG_RADIO_CONF_SEND_LENGTH;
  UINT16 flashPage      = FLASH_PAGE_SYSTEM_DATA;

  UINT8 retVal=FAILED;

  if(FlashInterface_Load_SystemData_FromFlash(dataRead, flashPage, dataOffset, numBytesToRead) == OK)
  {
    //load data from flash
    current_Radio_Conf_Data.freq_Band   = dataRead[0];
    current_Radio_Conf_Data.modulation  = dataRead[1];
    current_Radio_Conf_Data.dataRate    = dataRead[2];
    current_Radio_Conf_Data.channel     = dataRead[3];
    current_Radio_Conf_Data.power       = dataRead[4];

    retVal = OK;
  }
  else
  {
    //load mote default parameters if flash data load fails
    current_Radio_Conf_Data.freq_Band   = 0; //868 Mhz European band
    current_Radio_Conf_Data.modulation  = 5; //4GFSK- BT=1
    current_Radio_Conf_Data.dataRate    = 4; //500 Kbps
    current_Radio_Conf_Data.channel     = 0; //channel 0
    current_Radio_Conf_Data.power       = 128; //0 dBm

    retVal = FAILED;
  }

  //TODO
  Radio_Interface_Load_Parameters(current_Radio_Conf_Data);

  return retVal;
}

//*****************************************************************************
UINT8 CnfManager_Load_Mote_Default_Config(void)
//*****************************************************************************
// Loads current system config
//*****************************************************************************
{
  UINT8 retVal=FAILED;

  if( (CnfManager_Load_Mote_Identity_Config() == OK)    &&
      (Cnf_Manager_Load_Mote_Health_Config() == OK)     &&
      (Cnf_Manager_Load_ADC_Calibration_Config() == OK) &&
      (Cnf_Manager_Load_Radio_Config() == OK)
    )
  {
    retVal = OK;
  }
  else
  {
    retVal=FAILED;
  }

  return retVal;
}

//*****************************************************************************
UINT8 CnfManager_Get_My_Address(void)
//*****************************************************************************
// gets the current address of the mote
//*****************************************************************************
{
  UINT8 retVal=0;
  retVal = (UINT8)(current_Identity_Config.NodeID & 0x00FF);
  return retVal;
}

//*****************************************************************************
UINT16 CnfManager_Get_Health_Msg_Period(void)
//*****************************************************************************
// gets the current address of the mote
//*****************************************************************************
{
  return current_Health_Config.healthPeriod;
}

//*****************************************************************************
UINT8 CnfManager_Update_Node_Config(UINT8 dataConfigToBeUpdated)
//*****************************************************************************
// Sinchroniu
//*****************************************************************************
{
  UINT8 retVal=FAILED;

  switch(dataConfigToBeUpdated)
  {
    case IDENTITY_CONFIG_NODE_DATA:
      retVal = CnfManager_Load_Mote_Identity_Config();
    break;

    case HEALTH_CONFIG_NODE_DATA:
      retVal = Cnf_Manager_Load_Mote_Health_Config();
    break;

    case ADC_CONFIG_CALIBRATION_DATA:
      retVal = Cnf_Manager_Load_ADC_Calibration_Config();
    break;

    case RADIO_CONFIG_NODE_DATA:
      retVal = Cnf_Manager_Load_Radio_Config();
    break;

    default:
    break;
  }

  return retVal;
}

//*****************************************************************************
void CnfManager_Save_Data(void *dataBuffer, UINT16 msgType)
//*****************************************************************************
// Perfom a persistent copy of the data into the system
//*****************************************************************************
{
  UINT8 dataOffset = 0;
  UINT8 numBytesToWrite = 0;
  UINT16 flashPage=0;

  switch(msgType)
  {
    case MSG_IDENTITY_SEND:
      dataOffset = MSG_IDENTITY_FLASH_OFFSET;
      numBytesToWrite = MSG_IDENTITY_SEND_LENGTH;
      flashPage = FLASH_PAGE_SYSTEM_DATA;
    break;

    case MSG_HEALTH_CONF_SEND:
      dataOffset = MSG_HEALTH_CONF_FLASH_OFFSET;
      numBytesToWrite = MSG_HEALTH_CONF_SEND_LENGTH;
      flashPage = FLASH_PAGE_SYSTEM_DATA;
    break;

    case MSG_ADC_CALIBRATION_SEND:
      dataOffset = MSG_ADC_CALIBRATION_FLASH_OFFSET;
      numBytesToWrite = MSG_ADC_CAL_SEND_LENGTH;
      flashPage = FLASH_PAGE_SYSTEM_DATA;
      break;

    case MSG_RADIO_DATA_SEND:
      dataOffset = MSG_RADIO_CONFIG_FLASH_OFFSET;
      numBytesToWrite = MSG_RADIO_CONF_SEND_LENGTH;
      flashPage = FLASH_PAGE_SYSTEM_DATA;
      break;

    //TODO: program the rest of message data to be saved

    default:
    break;
  }

  FlashInterface_Save_SystemData_InFlash(dataBuffer, flashPage, dataOffset, numBytesToWrite);
}

//*****************************************************************************
UINT8 CnfManager_Load_Data(void *dataBuffer, UINT16 msgType)
//*****************************************************************************
// retrieves a persistent copy of the data
//*****************************************************************************
{
  UINT8 dataOffset = 0;
  UINT8 numBytesToRead = 0;
  UINT16 flashPage=0;

  switch(msgType)
  {
    case MSG_IDENTITY_RETRIEVE:
      dataOffset = MSG_IDENTITY_FLASH_OFFSET;
      numBytesToRead = MSG_IDENTITY_SEND_LENGTH;
      flashPage = FLASH_PAGE_SYSTEM_DATA;
    break;

    case MSG_HEALTH_RETRIEVE:
      //retrieving the health period which is the only data from health that its been stored in persistence
      dataOffset = MSG_HEALTH_CONF_FLASH_OFFSET;
      numBytesToRead = MSG_HEALTH_CONF_SEND_LENGTH;
      flashPage = FLASH_PAGE_SYSTEM_DATA;
    break;

    case MSG_ADC_CALIBRATION_RETRIEVE:
      dataOffset = MSG_ADC_CALIBRATION_FLASH_OFFSET;
      numBytesToRead = MSG_ADC_CAL_SEND_LENGTH;
      flashPage = FLASH_PAGE_SYSTEM_DATA;
      break;

    case MSG_RADIO_DATA_RETRIEVE:
      dataOffset = MSG_RADIO_CONFIG_FLASH_OFFSET;
      numBytesToRead = MSG_RADIO_CONF_SEND_LENGTH;
      flashPage = FLASH_PAGE_SYSTEM_DATA;
      break;

    //TODO: program the rest of message data to be saved

    default:
    break;
  }

  return FlashInterface_Load_SystemData_FromFlash(dataBuffer, flashPage, dataOffset, numBytesToRead);
}


