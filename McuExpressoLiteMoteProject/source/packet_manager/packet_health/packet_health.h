/*
 * packet_health.h
 *
 *  Created on: 13 may. 2020
 *      Author: MAX PC
 */

#ifndef PACKET_MANAGER_PACKET_HEALTH_PACKET_HEALTH_H_
#define PACKET_MANAGER_PACKET_HEALTH_PACKET_HEALTH_H_

#include "globals.h"

#define MSG_HEALTH_SEND_LENGTH         sizeof(TMote_Health_Data)
#define MSG_HEALTH_RETRIEVE_LENGTH     0

typedef struct __attribute__((packed)) TMote_Health_Data
{
  UINT16 vddVoltage;
  UINT16 batVoltage;
  UINT16 temp;
  UINT16 healthPeriod; //in seconds
}TMote_Health_Data;

#endif /* PACKET_MANAGER_PACKET_HEALTH_PACKET_HEALTH_H_ */
