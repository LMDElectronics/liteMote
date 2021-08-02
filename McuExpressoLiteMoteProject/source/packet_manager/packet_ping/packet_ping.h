/*
 * packet_ping.h
 *
 *  Created on: 16 feb. 2019
 *      Author: MAX PC
 */

#ifndef PACKET_MANAGER_PACKET_PING_PACKET_PING_H_
#define PACKET_MANAGER_PACKET_PING_PACKET_PING_H_

#include <Serial_Manager/Serial_Packet_FIFOs.h>
#include "globals.h"

#define MSG_PING_LENGTH     sizeof(TMote_Ping_Data)
#define PING_PAYLOAD_VALUE  0xAAAA

typedef struct __attribute__((packed)) TMote_Ping_Data
{
  UINT16 ping_data;
}TMote_Ping_Data;

TS_packet Build_Ping_Packet_Serial(TS_packet serial_packet_to_process);

#endif /* PACKET_MANAGER_PACKET_PING_PACKET_PING_H_ */
