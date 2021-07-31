/*
 * packet_ack.h
 *
 *  Created on: 9 may. 2020
 *      Author: MAX PC
 */

#ifndef PACKET_MANAGER_PACKET_ACK_PACKET_ACK_H_
#define PACKET_MANAGER_PACKET_ACK_PACKET_ACK_H_

#include <Serial_Manager/Serial_Packet_FIFOs.h>
#include "globals.h"

#define MSG_ACK_LENGTH             2

typedef struct __attribute__((packed)) TMote_Ack_Data
{
  UINT16 msgTypeAcked;
}TMote_Ack_Data;


#endif /* PACKET_MANAGER_PACKET_ACK_PACKET_ACK_H_ */
