/*
 * packet_manager.h
 *
 *  Created on: 16 feb. 2019
 *      Author: MAX PC
 */

#ifndef PACKET_MANAGER_PACKET_MANAGER_H_
#define PACKET_MANAGER_PACKET_MANAGER_H_

#include <Serial_Manager/Serial_Packet_FIFOs.h>
#include "packet_ack/packet_ack.h"
#include "packet_ping/packet_ping.h"
#include "packet_identity/packet_identity.h"
#include "packet_health/packet_health.h"
#include "packet_health_conf/packet_health_conf.h"
#include "packet_adc_calibration/packet_adc_cal_val.h"
#include "packet_radio_conf/packet_radio_conf.h"

enum
{
  PACKET_MANAGER_CHECK_FOR_SERIAL_RX_PACKETS = 0,
  PACKET_MANAGER_CHECK_FOR_RADIO_RX_PACKETS,
};

#define HOST_SERIAL_ADDR 0

//data msg types
enum
{
  MSG_PING = 1,

  MSG_IDENTITY_RETRIEVE = 2,
  MSG_IDENTITY_SEND = 3,

  MSG_HEALTH_RETRIEVE = 4,
  MSG_HEALTH_SEND = 5,
  MSG_HEALTH_CONF_SEND = 6,

  MSG_ADC_CALIBRATION_RETRIEVE = 7,
  MSG_ADC_CALIBRATION_SEND = 8,

  MSG_RADIO_DATA_RETRIEVE = 9,
  MSG_RADIO_DATA_SEND = 10,

  MSG_ACK = 254,
};

UINT8 Packet_Manager_Init(void);
void Packet_Manager_Process_Motor(void);

TS_packet Build_Packet_Serial(UINT8 *payload, UINT16 msgType);

#endif /* PACKET_MANAGER_PACKET_MANAGER_H_ */
