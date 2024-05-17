/*
 * Serial_Packet_FIFO.h
 *
 *  Created on: 1 feb. 2019
 *      Author: MAX PC
 */

#ifndef _SERIAL_PACKET_FIFOS_H_
#define _SERIAL_PACKET_FIFOS_H_

#include "globals.h"
#include <packet_manager/packet_manager.h>

void Serial_Packet_Rx_FIFO_Init(void);
Tpacket Get_Serial_Rx_FIFO_Packet(void);
void Push_Serial_Rx_FIFO_Packet(Tpacket serial_packet_To_Push);
UINT8 Is_Serial_Rx_FIFO_Empty(void);

void Serial_Packet_Tx_FIFO_Init(void);
Tpacket Get_Serial_Tx_FIFO_Packet(void);
void Push_Serial_Tx_FIFO_Packet(Tpacket serial_packet_To_Push);
UINT8 Is_Serial_Tx_FIFO_Empty(void);


#endif /* 01___SERIAL_MANAGER_SERIAL_PACKET_FIFOS_H_ */
