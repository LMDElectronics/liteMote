/*
 * packet_manager.h
 *
 *  Created on: 16 feb. 2019
 *      Author: MAX PC
 */

#ifndef PACKET_MANAGER_PACKET_MANAGER_H_
#define PACKET_MANAGER_PACKET_MANAGER_H_

typedef struct TpacketHeader
{
  UINT16 origin_node;
  UINT16 destination_node;
  UINT16 send_time;
  UINT16 msg_type;
  UINT8  frame_payload_length;
}TpacketHeader;

#define MAX_PACKET_SIZE 128
#define MAX_PAYLOAD_BYTES ( MAX_PACKET_SIZE - sizeof(TpacketHeader) - 2) // 2 CRC bytes
#define MAX_PACKETS_ALLOWED 4 //max packets allowed to be enqueud in FIFO

typedef struct Tpacket
{
	TpacketHeader header;
  UINT8 payload[MAX_PAYLOAD_BYTES];
}Tpacket;

/*
  data sorting:

  data buffer sorted [origin_node_MSB][origin_node_LSB]...[msg_type]
  buffer bytes       [0][1][2]...[N]
*/

enum
{
  PACKET_MANAGER_CHECK_FOR_SERIAL_RX_PACKETS = 0,
  PACKET_MANAGER_CHECK_FOR_RADIO_RX_PACKETS,
};

#define HOST_SERIAL_ADDR 0

//packet data msg types
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

Tpacket Build_Packet_Serial(UINT8 *payload, UINT16 msgType);
Tpacket Build_Packet_Radio(UINT8 origin_address, UINT8 destination_Address, UINT16 send_time, UINT16 msgType, UINT8 *payload);

#endif /* PACKET_MANAGER_PACKET_MANAGER_H_ */
