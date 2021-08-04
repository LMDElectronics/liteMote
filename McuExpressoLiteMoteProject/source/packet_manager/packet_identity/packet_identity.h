/*
 * packet_identity.h
 *
 *  Created on: 7 may. 2020
 *      Author: MAX PC
 */

#ifndef PACKET_MANAGER_PACKET_IDENTITY_PACKET_IDENTITY_H_
#define PACKET_MANAGER_PACKET_IDENTITY_PACKET_IDENTITY_H_

#include <Serial_Manager/Serial_Packet_FIFOs.h>
#include "globals.h"

#define MSG_IDENTITY_SEND_LENGTH         sizeof(TMote_Identity_Data)
#define MSG_IDENTITY_RETRIEVE_LENGTH     0

typedef struct TMote_Identity_Data
{
  UINT16 NodeID;
  UINT16 NodeType;
  UINT16 fw_version;
  UINT16 fw_CRC;
  UINT16 hw_version;
  UINT16 hw_case;

}TMote_Identity_Data;

#endif /* PACKET_MANAGER_PACKET_IDENTITY_PACKET_IDENTITY_H_ */
