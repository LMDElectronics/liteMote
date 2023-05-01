/*
 * Serial_Manager.h
 *
 *  Created on: 1 feb. 2019
 *      Author: MAX PC
 */

#ifndef SERIAL_MANAGER_SERIAL_MANAGER_H_
#define SERIAL_MANAGER_SERIAL_MANAGER_H_

#include <Serial_Manager/Serial_Packet_FIFOs.h>
#include "globals.h"

enum
{
  SERIAL_MANAGER_WAIT_ENCODED_START_FRAME = 0,
  SERIAL_MANAGER_GET_ENCODED_FRAME,
  SERIAL_MANAGER_DECODE_SERIAL_FRAME,
};

enum
{
  SERIAL_MANAGER_TX_CHECK_TO_SEND = 0,
  SERIAL_MANAGER_TX_WAIT_FOR_TX_COMPLETED,
};

//data size
#define DATA_BUFFER_ORIGIN_NODE_SIZE_BYTES      2
#define DATA_BUFFER_DESTINATION_NODE_SIZE_BYTES 2
#define DATA_BUFFER_SEND_TIME_SIZE_BYTES        2
#define DATA_BUFFER_MSG_TYPE_SIZE_BYTES         2
#define DATA_BUFFER_PAYLOAD_LENGTH_SIZE_BYTES   1

//data offsets, to build serial packets from serial frames
#define DATA_BUFFER_ORIGIN_NODE_OFFSET      0
#define DATA_BUFFER_DESTINATION_NODE_OFFSET 2
#define DATA_BUFFER_SEND_TIME_OFFSET        4
#define DATA_BUFFER_MSG_TYPE_OFFSET         6
#define DATA_BUFFER_PAYLOAD_LENGTH_OFFSET   8
#define DATA_BUFFER_PAYLOAD_START_OFFSET    9

UINT8 Serial_Manager_Init(void);
TS_packet From_Serial_Frame_To_Packet(UINT8 *dataBuffer);
UINT8 From_Packet_To_Serial_Frame(TS_packet serial_tx_packet, UINT8 *dataBuffer);
void Serial_Manager_RX_Motor(void);
void Serial_Manager_Tx_Motor(void);

#endif /* 01___SERIAL_MANAGER_SERIAL_MANAGER_H_ */
