/*
 * Radio_Packet_FIFOs.h
 *
 *  Created on: 2 ago. 2021
 *      Author: MAX PC
 */

#ifndef RADIOMANAGER_RADIO_PACKET_FIFOS_H_
#define RADIOMANAGER_RADIO_PACKET_FIFOS_H_

#include "globals.h"

#define MAX_RADIO_PAYLOAD       128

typedef struct TR_packet_header
{
  UINT8 origin_node;
  UINT8 destination_node;
  UINT16 send_time;
  UINT8 ackNeeded;
  UINT8  frame_payload_length;
}TR_packet_header;

typedef struct TR_packet
{
  TR_packet_header header;
  UINT8 payload[MAX_RADIO_PAYLOAD];

}TR_packet;

#define MAX_RADIO_PACKETS_ALLOWED 4 //max packets allowed to be enqueud in FIFO

void Radio_Packet_Rx_FIFO_Init(void);
TR_packet Get_Radio_Rx_FIFO_Packet(void);
void Push_Radio_Rx_FIFO_Packet(TR_packet radio_packet_To_Push);
UINT8 Is_Radio_Rx_FIFO_Empty(void);

void Radio_Packet_Tx_FIFO_Init(void);
TR_packet Get_Radio_Tx_FIFO_Packet(void);
void Push_Radio_Tx_FIFO_Packet(TR_packet radio_packet_To_Push);
UINT8 Is_Radio_Tx_FIFO_Empty(void);

#endif /* RADIOMANAGER_RADIO_PACKET_FIFOS_H_ */
