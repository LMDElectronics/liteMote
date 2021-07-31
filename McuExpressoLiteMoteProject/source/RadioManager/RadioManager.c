/*
 * RadioManager.c
 *
 *  Created on: 4 jun. 2020
 *      Author: MAX PC
 */

#include <RadioManager/RadioManager.h>
#include "External_HW_Drivers/s2lp/s2lp.h"

//*****************************************************************************
void Radio_Manager_Config(void)
//*****************************************************************************
//
//*****************************************************************************
{

}

//*****************************************************************************
void Radio_Interface_Load_Parameters(TMote_Radio_Conf_Data current_Radio_Conf_Data)
//*****************************************************************************
// Load the radio parameters into the radio device
//*****************************************************************************
{
  switch(current_Radio_Conf_Data.freq_Band)
  {
    case EURO_FREQ_CENTER:      S2lp_Set_Base_Center_Freq(EUROPEAN_FREQ_BAND);  break;
    case AMERICAN_FREQ_CENTER:  S2lp_Set_Base_Center_Freq(AMERICAN_FREQ_BAND);  break;
    default: break;
  }

  switch(current_Radio_Conf_Data.modulation)
  {
    case INDEXED_2FSK:          s2lp_Set_Modulation_Type(TWO_FSK);              break;
    case INDEXED_4FSK:          s2lp_Set_Modulation_Type(FOUR_FSK);             break;
    case INDEXED_2GFSK_BT_1:    s2lp_Set_Modulation_Type(TWO_GFSK_BT_1);        break;
    case INDEXED_4GFSK_BT_1:    s2lp_Set_Modulation_Type(FOUR_GFSK_BT_1);       break;
    case INDEXED_ASK_OOK:       s2lp_Set_Modulation_Type(ASK_OOK);              break;
    case INDEXED_2GFSK_BT_05:   s2lp_Set_Modulation_Type(TWO_GFSK_BT_05);       break;
    case INDEXED_4GFSK_BT_05:   s2lp_Set_Modulation_Type(FOUR_GFSK_BT_05);      break;
    default: break;
  }

  switch(current_Radio_Conf_Data.dataRate)
  {
    case INDEXED_DATA_RATE_25_KBPS: s2lp_Set_DataRate(DATA_RATE_25_KBPS);     break;
    case INDEXED_DATA_RATE_50_KBPS: s2lp_Set_DataRate(DATA_RATE_50_KBPS);     break;
    case INDEXED_DATA_RATE_100_KBPS: s2lp_Set_DataRate(DATA_RATE_100_KBPS);     break;
    case INDEXED_DATA_RATE_200_KBPS: s2lp_Set_DataRate(DATA_RATE_200_KBPS);     break;
    case INDEXED_DATA_RATE_300_KBPS: s2lp_Set_DataRate(DATA_RATE_300_KBPS);     break;
    case INDEXED_DATA_RATE_400_KBPS: s2lp_Set_DataRate(DATA_RATE_400_KBPS);     break;
    case INDEXED_DATA_RATE_500_KBPS: s2lp_Set_DataRate(DATA_RATE_500_KBPS);     break;
    default: break;
  }

  s2lp_Set_Channel_Num(current_Radio_Conf_Data.channel);
  s2lp_Set_Tx_Power_Config(current_Radio_Conf_Data.power);
}

//****************************************************************************
void Radio_Manager_Init(void)
//****************************************************************************
// Inits the radio Manager
//****************************************************************************
{
  S2lp_Init();
}

//****************************************************************************
void Radio_Manager_Tx_Task(void)
//*****************************************************************************
// Motor to Tx the radio packets ready
//*****************************************************************************
{


}



















