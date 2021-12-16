/*
 * RadioManager.h
 *
 *  Created on: 4 jun. 2020
 *      Author: MAX PC
 */

#ifndef RADIOMANAGER_RADIOMANAGER_H_
#define RADIOMANAGER_RADIOMANAGER_H_

#include "globals.h"
#include <RadioManager/Radio_Packet_FIFOs.h>
#include "packet_manager/packet_radio_conf/packet_radio_conf.h"

#define BOARD_TPM TPM0

#define TPM_SOURCE_CLOCK (CLOCK_GetFreq(kCLOCK_McgInternalRefClk)/4)
//#define TPM_SOURCE_CLOCK (CLOCK_GetFreq(kCLOCK_PllFllSelClk)/4)

#define TPM_PRESCALER kTPM_Prescale_Divide_4
#define BOARD_TPM_IRQ_NUM TPM0_IRQn
#define BOARD_TPM_HANDLER TPM0_IRQHandler

#define NO_ACK_NEEDED   0
#define ACK_NEEDED      1

enum
{
  RADIO_MANAGER_TX_CHECK_TO_SEND = 0,
  RADIO_MANAGER_TX_SENDING_PACKET,
  RADIO_MANAGER_TX_FINISHED,
};

void Radio_Manager_Config(void);
void Radio_Manager_Init(void);
void Radio_Manager_Tx_Motor(void);
void Radio_Manager_Rx_Motor(void);

//tx window function
void Radio_Tx_Window_Timer_Init(void);
UINT8 Is_Send_Timer_Timeout_Flag_Set(void);
void Radio_Window_Timer_Set_Tx_Window(UINT16 timeout);
void Radio_Window_Timer_Start_Timer(void);
void Radio_Manager_Load_Packet(UINT8 myAddress, UINT8 destination_addr, UINT8 *payload, UINT8 payloadLength, UINT8 ackNeeded);

#endif /* RADIOMANAGER_RADIOMANAGER_H_ */
