/*
 * Serial_Packet_FIFO.h
 *
 *  Created on: 1 feb. 2019
 *      Author: MAX PC
 */

#ifndef SERIAL_MANAGER_SERIAL_PACKET_FIFOS_H_
#define SERIAL_MANAGER_SERIAL_PACKET_FIFOS_H_

#include "globals.h"

#define MAX_PACKET_SIZE 128
#define MAX_PAYLOAD_BYTES ( MAX_PACKET_SIZE - sizeof(TS_packet_header) - 2) // 2 CRC bytes

#define MAX_PACKETS_ALLOWED 4 //max packets allowed to be enqueud in FIFO

typedef struct TS_packet_header
{
  UINT8 origin_node;
  UINT8 destination_node;
  UINT16 send_time;
  UINT16 msg_type;
  UINT8  frame_payload_length;
}TS_packet_header;

typedef struct TS_packet
{
  TS_packet_header header;
  UINT8 payload[MAX_PAYLOAD_BYTES];

}TS_packet;

/*
  data sorting:

  data buffer sorted [origin_node_MSB][origin_node_LSB]...[msg_type]
  buffer bytes       [0][1][2]...[N]
*/

void Serial_Packet_Rx_FIFO_Init(void);
TS_packet Get_Serial_Rx_FIFO_Packet(void);
void Push_Serial_Rx_FIFO_Packet(TS_packet serial_packet_To_Push);
UINT8 Is_Serial_Rx_FIFO_Empty(void);

void Serial_Packet_Tx_FIFO_Init(void);
TS_packet Get_Serial_Tx_FIFO_Packet(void);
void Push_Serial_Tx_FIFO_Packet(TS_packet serial_packet_To_Push);
UINT8 Is_Serial_Tx_FIFO_Empty(void);


#endif /* 01___SERIAL_MANAGER_SERIAL_PACKET_FIFOS_H_ */
