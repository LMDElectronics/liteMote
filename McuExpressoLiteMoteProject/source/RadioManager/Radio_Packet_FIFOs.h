/*
 * Radio_Packet_FIFOs.h
 *
 *  Created on: 2 ago. 2021
 *      Author: MAX PC
 */

#ifndef RADIOMANAGER_RADIO_PACKET_FIFOS_H_
#define RADIOMANAGER_RADIO_PACKET_FIFOS_H_

#include "globals.h"
#include <packet_manager/packet_manager.h>

void Radio_Packet_Rx_FIFO_Init(void);
Tpacket Get_Radio_Rx_FIFO_Packet(void);
void Push_Radio_Rx_FIFO_Packet(Tpacket radio_packet_To_Push);
UINT8 Is_Radio_Rx_FIFO_Empty(void);

void Radio_Packet_Tx_FIFO_Init(void);
Tpacket Get_Radio_Tx_FIFO_Packet(void);
void Push_Radio_Tx_FIFO_Packet(Tpacket radio_packet_To_Push);
UINT8 Is_Radio_Tx_FIFO_Empty(void);

#endif /* RADIOMANAGER_RADIO_PACKET_FIFOS_H_ */
