/*
 * RadioManager.c
 *
 *  Created on: 4 jun. 2020
 *      Author: MAX PC
 */

#include <RadioManager/RadioManager.h>
#include "External_HW_Drivers/s2lp/s2lp.h"
#include "fsl_port.h"
#include "fsl_tpm.h"
#include "fsl_common.h"
#include "fsl_port.h"
#include "fsl_gpio.h"

UINT8 radio_manager_Tx_state = RADIO_MANAGER_TX_CHECK_TO_SEND;
TR_packet radio_packet_to_Tx;

volatile bool tpmIsrFlag = FALSE;

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
  Radio_Window_Timer_Init();

  radio_manager_Tx_state = RADIO_MANAGER_TX_CHECK_TO_SEND;
}

//****************************************************************************
void Radio_Manager_Load_Packet(UINT8 destination_addr, UINT8 payload)
//****************************************************************************
// Loads the packet into radio transmitter
//****************************************************************************
{

}

//****************************************************************************
void Radio_Manager_Tx_Motor(void)
//*****************************************************************************
// Motor to Tx the radio packets ready
//*****************************************************************************
{
  switch(radio_manager_Tx_state)
  {
    case RADIO_MANAGER_TX_CHECK_TO_SEND:

      if(Is_Radio_Tx_FIFO_Empty() == TRUE)
      {
        //nothing to do radio fifo empty
      }
      else
      {
        radio_packet_to_Tx = Get_Radio_Tx_FIFO_Packet();

        //1 - Load Radio packet
        Radio_Manager_Load_Packet(radio_packet_to_Tx.header.destination_node, radio_packet_to_Tx.payload);

        //2 - load send time timer
        Radio_Window_Timer_Set_Tx_Window(radio_packet_to_Tx.header.send_time);

        //3 - start timer
        tpmIsrFlag = FALSE; //reset isr flag
        Radio_Window_Timer_Start_Timer();

        //4 - start tx
        //s2lp_Start_Tx();

        radio_manager_Tx_state = RADIO_MANAGER_TX_SENDING_PACKET;
      }

    break;

    case RADIO_MANAGER_TX_SENDING_PACKET:

      if(Is_Send_Timer_Timeout_Flag_Set())
      {
        //TODO:
        //send time window finish, stop Tx
        //s2lp_Stop_Tx();
        //start_Tx_Window_Timer();

        radio_manager_Tx_state = RADIO_MANAGER_TX_FINISHED;
      }
      else
      {
        if( s2lp_Get_Operating_State() == STATE_READY )
        {
          //Tx timeout window not reached, start Tx again
          //s2lp_Start_Tx();
        }

        radio_manager_Tx_state = RADIO_MANAGER_TX_SENDING_PACKET;
      }
    break;

    case RADIO_MANAGER_TX_FINISHED:
      //clean state
      radio_manager_Tx_state = RADIO_MANAGER_TX_CHECK_TO_SEND;
    break;

    default:
    break;
  }
}

//*****************************************************************************
// Tx send timer functions
//*****************************************************************************

//*****************************************************************************
void Radio_Window_Timer_Init(void)
//*****************************************************************************
// Inits the timer controlling the Tx window time
//*****************************************************************************
{
  tpm_config_t tpmInfo;

  //test
  /*gpio_pin_config_t wp_config = {kGPIO_DigitalOutput, 1};
  PORT_SetPinMux(PORTC, 10U, kPORT_MuxAsGpio);
  GPIO_PinInit(GPIOC, 10, &wp_config);
  GPIO_PortClear(GPIOC, 1u << 10);*/

  //enabling clcok gate for tpm peripheral
  //CLOCK_SetTpmClock(1U);
  CLOCK_SetTpmClock(3U);

  TPM_GetDefaultConfig(&tpmInfo);
  tpmInfo.prescale = TPM_PRESCALER;

  //init tpm peripheral
  TPM_Init(BOARD_TPM, &tpmInfo);

  //enabling interrupts for tpm0
  TPM_EnableInterrupts(BOARD_TPM, kTPM_TimeOverflowInterruptEnable);
  EnableIRQ(BOARD_TPM_IRQ_NUM);
}

//*****************************************************************************
void Radio_Window_Timer_Set_Tx_Window(UINT16 timeout)
//*****************************************************************************
// Sets the timeWindow in which the radio will be transmitting the current packet
// to be transmitted
//*****************************************************************************
{
  TPM_SetTimerPeriod(BOARD_TPM, MSEC_TO_COUNT(timeout, TPM_SOURCE_CLOCK));
}

//*****************************************************************************
void Radio_Window_Timer_Start_Timer(void)
//*****************************************************************************
{
  TPM_StartTimer(BOARD_TPM, kTPM_SystemClock);
  //GPIO_PortSet(GPIOC, 1u << 10);
}

//*****************************************************************************
void Radio_Window_Timer_Stop_Timer(void)
//*****************************************************************************
{
  TPM_StopTimer(BOARD_TPM);
}

//*****************************************************************************
UINT8 Is_Send_Timer_Timeout_Flag_Set(void)
//*****************************************************************************
// Checks if radio Tx window has finished
//*****************************************************************************
{
   return tpmIsrFlag;
}

//*****************************************************************************
//void Radio_Manager_ISR
//*****************************************************************************
// Motor to Rx the radio packets received
//*****************************************************************************
void BOARD_TPM_HANDLER(void)
{
  /* Clear interrupt flag.*/
  //GPIO_PortClear(GPIOC, 1u << 10);

  TPM_ClearStatusFlags(BOARD_TPM, kTPM_TimeOverflowFlag);
  tpmIsrFlag = true;

  //stop timer
  TPM_StopTimer(BOARD_TPM);

  __DSB();
}

















