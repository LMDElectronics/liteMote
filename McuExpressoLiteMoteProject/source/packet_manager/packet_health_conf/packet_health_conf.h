/*
 * packet_health_conf.h
 *
 *  Created on: 13 may. 2020
 *      Author: MAX PC
 */

#ifndef PACKET_MANAGER_PACKET_HEALTH_CONF_PACKET_HEALTH_CONF_H_
#define PACKET_MANAGER_PACKET_HEALTH_CONF_PACKET_HEALTH_CONF_H_

#include "globals.h"

#define MSG_HEALTH_CONF_SEND_LENGTH         sizeof(TMote_Health_Conf_Data)
#define MSG_HEALTH_CONF_RETRIEVE_LENGTH     0

typedef struct TMote_Health_Conf_Data
{
  UINT16 healthPeriod; //minutes
}TMote_Health_Conf_Data;

#endif /* PACKET_MANAGER_PACKET_HEALTH_CONF_PACKET_HEALTH_CONF_H_ */
