/*
 * health_manager.c
 *
 *  Created on: 13 may. 2020
 *      Author: MAX PC
 */

#include <Configuration_Manager/configuration_manager.h>
#include "health_manager.h"
#include "packet_manager/packet_manager.h"

UINT8 Health_Manager_State;
TS_packet packet_aux;

//*****************************************************************************
void Health_Manager_Motor(void)
//*****************************************************************************
// Motor to check if health message should be sent
//*****************************************************************************
{
  switch(Health_Manager_State)
  {
    case IDLE_STATE:

      if(Get_Tx_Health_Flag() == TRUE)
      {
        Reset_Tx_Health_Flag();
        Health_Manager_State = SEND_TIMED_HEALTH_MESSAGE;
      }

      break;

    case SEND_TIMED_HEALTH_MESSAGE:

      Health_Manager_Set_Data(packet_aux.payload);
      Push_Serial_Tx_FIFO_Packet(Build_Packet_Serial(packet_aux.payload, MSG_HEALTH_SEND));

      Health_Manager_State = IDLE_STATE;

      break;
  }
}

//*****************************************************************************
void Health_Manager_Init(void)
//*****************************************************************************
// Inits the health manager module
//*****************************************************************************
{
  Health_Manager_State = IDLE_STATE;
}

//*****************************************************************************
UINT16 Health_Manager_Get_Bat_Voltage(void)
//*****************************************************************************
// gets Battery voltage in V
//*****************************************************************************
{
   UINT16 batVoltage_mV=0;

   ADC_Init();

   //TODO set a channel to monitor the battery voltage or check frdm board schemtaics

   ADC_DeInit();
   ADC_Disable();

   return batVoltage_mV=0;
}

//*****************************************************************************
UINT16 Health_Manager_Get_Voltage_Ref(void)
//*****************************************************************************
// gets ref voltage in V
//*****************************************************************************
{
  float refVoltage=0;
  UINT16 data=0;

  ADC_Init();

  //TODO set a channel to monitor the battery voltage or check frdm board schemtaics
  ADC_StartConversion(VREF_ADC_CHANNEL);
  while(ADC_WaitForConversion());
  ADC_Disable();

  refVoltage = (((ADC_GetData()) * VREF_V)/65535);

  ADC_DeInit();

  //data MSB unit part
  //data LSB decimal part
  data = (UINT8)refVoltage;
  refVoltage = (refVoltage - data)*100;
  data <<= 8;
  data |= (UINT8)refVoltage;

  return data;
}

//*****************************************************************************
UINT16 Health_Manager_Get_Temperature(void)
//*****************************************************************************
// gets temperature MAX=128, MIN =-128 UINT16 but its really CA2
//*****************************************************************************
{
   float temp=0;
   UINT16 data=0;

   ADC_Init();

   //TODO set a channel to monitor the battery voltage or check frdm board schemtaics
   ADC_StartConversion(TEMP_ADC_CHANNEL);
   while(ADC_WaitForConversion());
   ADC_Disable();

   //kl82 datasheet data extracted
   //0.0503mv one LSB (CPU_Voltage/(2^16-1) = (3.3/65535))
   //706 minimum mv sensor voltage at 25º
   //1.69 maximum mV/cº
   temp = (25 - (((ADC_GetData() * 0.0503547) - 706 )/(1.69)));

   ADC_DeInit();

   //data MSB unit part
   //data LSB decimal part
   if(temp < 0)
   {
     temp = -temp;

     data = (UINT8)temp;
     temp = (temp - data)*100;
     data <<= 8;
     data |= (UINT8)temp;

     data |= 0x8000; //adding sign
   }
   else
   {
     data = (UINT8)temp;
     temp = (temp - data)*100;
     data <<= 8;
     data |= (UINT8)temp;
   }

   return data;
 }

//*****************************************************************************
void Health_Manager_Set_Data(UINT8 *bufferToFill)
//*****************************************************************************
// sets the data in a buffer
//*****************************************************************************
{
  UINT16 batVoltage = 0;
  UINT16 refVoltage = 0;
  UINT16 temp = 0;

  //load msg period directly into buffer
  CnfManager_Load_Data(bufferToFill, MSG_HEALTH_RETRIEVE);

  //translating the persistent health msg data period to its corresponding place in the buffer to be uploaded into a packet
  bufferToFill[6] = bufferToFill[0];
  bufferToFill[7] = bufferToFill[1];
  bufferToFill[8] = bufferToFill[2];
  bufferToFill[9] = bufferToFill[3];

  //load health data
  refVoltage = Health_Manager_Get_Voltage_Ref();
  batVoltage = Health_Manager_Get_Bat_Voltage();
  temp = Health_Manager_Get_Temperature();

  //adding health data to the buffer
  bufferToFill[0] = (UINT8)(refVoltage >> 8);
  bufferToFill[1] = (UINT8)(refVoltage & 0x00ff);

  bufferToFill[2] = (UINT8)(batVoltage >> 8);
  bufferToFill[3] = (UINT8)(batVoltage & 0x00ff);

  bufferToFill[4] = (UINT8)(temp >> 8);
  bufferToFill[5] = (UINT8)(temp & 0x00ff);
}

//*****************************************************************************
void Health_Manager_Reload_Timer(UINT16 timerCompValue)
//*****************************************************************************
// safely reload health counter
//*****************************************************************************
{
  Health_Timer_Stop();
  Health_Timer_Load_Counter_CMP(MINUTES_TO_COUNT(timerCompValue,1000,1024));
  Health_Timer_Start();
}

