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

UINT8 Is_Send_Timer_Timeout_Flag_Set(void);

#endif /* RADIOMANAGER_RADIOMANAGER_H_ */
