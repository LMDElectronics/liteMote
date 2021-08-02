/*
 * packet_manager.c
 *
 *  Created on: 16 feb. 2019
 *      Author: MAX PC
 */

#include "globals.h"
#include "packet_manager.h"
#include "Configuration_Manager/configuration_manager.h"
#include "Health_Manager/health_manager.h"

UINT8 packet_Manager_Motor_State = PACKET_MANAGER_CHECK_FOR_SERIAL_RX_PACKETS;

/******************************************************************************
UINT8 Packet_Manager_Init(void)
   Inits the packet manager

  Pre condition:
    None

  Post condition:

  Params:
    None

  Return:
    INIT_OK or INIT_FAILED

*******************************************************************************/
UINT8 Packet_Manager_Init(void)
{
  packet_Manager_Motor_State = PACKET_MANAGER_CHECK_FOR_SERIAL_RX_PACKETS;
  return INIT_OK;
}

/******************************************************************************
UINT8 Get_Canonical_Payload_length(UINT16 current_msg_type)
   Gets the length of payload according to msg type

  Pre condition:
    None

  Post condition:

  Params:
    None

  Return:
    INIT_OK or INIT_FAILED

*******************************************************************************/
UINT8 Get_Canonical_Payload_length(UINT16 current_msg_type)
{
  UINT8 length = 0;

  switch(current_msg_type)
  {
    case MSG_ACK:                       length = MSG_ACK_LENGTH;                    break;
    case MSG_PING:                      length = MSG_PING_LENGTH;                   break;

    case MSG_IDENTITY_SEND:             length = MSG_IDENTITY_SEND_LENGTH;          break;
    case MSG_IDENTITY_RETRIEVE:         length = MSG_IDENTITY_RETRIEVE_LENGTH;      break;

    case MSG_HEALTH_SEND:               length = MSG_HEALTH_SEND_LENGTH;            break;
    case MSG_HEALTH_RETRIEVE:           length = MSG_HEALTH_RETRIEVE_LENGTH;        break;

    case MSG_HEALTH_CONF_SEND:          length = MSG_HEALTH_CONF_SEND_LENGTH;       break;

    case MSG_ADC_CALIBRATION_SEND:      length = MSG_ADC_CAL_SEND_LENGTH;           break;
    case MSG_ADC_CALIBRATION_RETRIEVE:  length = MSG_ADC_CALHEALTH_RETRIEVE_LENGTH; break;

    case MSG_RADIO_DATA_SEND:           length = MSG_RADIO_CONF_SEND_LENGTH;        break;
    case MSG_RADIO_DATA_RETRIEVE:       length = MSG_RADIO_CONF_RETRIEVE_LENGTH;    break;

    default:                            length = 0;                                 break;
  }

  return length;
}

/******************************************************************************
TS_packet Build_Ping_Packet_Serial(void)
   Builds ping packet [serial format]

  Pre condition:
    None

  Post condition:
    None

  Params:
    None

  Return:
   formatted serial packet

*******************************************************************************/
TS_packet Build_Packet_Serial(UINT8 *payload, UINT16 msgType)
{
  UINT8 i=0;
  TS_packet serial_packet;

  serial_packet.header.origin_node = CnfManager_Get_My_Address();
  serial_packet.header.destination_node = HOST_SERIAL_ADDR;
  serial_packet.header.send_time = 0;
  serial_packet.header.msg_type = msgType;
  serial_packet.header.frame_payload_length = Get_Canonical_Payload_length(msgType);

  for(i=0; i < serial_packet.header.frame_payload_length; i++)
  {
    serial_packet.payload[i] = payload[i];
  }

  return serial_packet;
}

/******************************************************************************
void Process_Packet(TS_packet packet_to_process)
   Proces the serial packet

  Pre condition:
    None

  Post condition:

  Params:
    packet to process

  Return:
   None

*******************************************************************************/
void Process_Packet(TS_packet packet_to_process)
{
  TS_packet packet_aux;

  UINT8 data=0;

  //check msg payload length
  if(packet_to_process.header.frame_payload_length != Get_Canonical_Payload_length(packet_to_process.header.msg_type))
  {
    //discard packet, length error
  }
  else
  {
    //check destination
    if(packet_to_process.header.destination_node == CnfManager_Get_My_Address() || packet_to_process.header.origin_node == BACKDOOR_BASE_ADDR)
    {
      //serial packet is for this mote or comes from host, process the packet
      //check the crc

      switch(packet_to_process.header.msg_type)
      {
        case MSG_PING:
          Push_Serial_Tx_FIFO_Packet(Build_Packet_Serial(packet_to_process.payload, MSG_PING));
        break;

        case MSG_IDENTITY_SEND:

          //save mote identity configuration
          CnfManager_Save_Data(packet_to_process.payload, MSG_IDENTITY_SEND);

          //update node current identity data
          if(CnfManager_Update_Node_Config(IDENTITY_CONFIG_NODE_DATA) == OK)
          {
            //load ack payload which is the msgtype of the msg received and send ack
            packet_aux.payload[0] = (UINT8)(packet_to_process.header.msg_type & 0xff00) >> 8;
            packet_aux.payload[1] = (UINT8)(packet_to_process.header.msg_type & 0x00ff);

            Push_Serial_Tx_FIFO_Packet(Build_Packet_Serial(packet_aux.payload, MSG_ACK));
          }
        break;

        case MSG_IDENTITY_RETRIEVE:
          CnfManager_Load_Data(packet_aux.payload, MSG_IDENTITY_RETRIEVE);
          Push_Serial_Tx_FIFO_Packet(Build_Packet_Serial(packet_aux.payload, MSG_IDENTITY_SEND));
        break;

        case MSG_HEALTH_CONF_SEND:
          CnfManager_Save_Data(packet_to_process.payload, MSG_HEALTH_CONF_SEND);

          //update node current health msg period data
          if(CnfManager_Update_Node_Config(HEALTH_CONFIG_NODE_DATA) == OK)
          {
            //restart Health manager
            Health_Manager_Init();

            //load ack payload which is the msgtype of the msg received and send ack
            packet_aux.payload[0] = (UINT8)(packet_to_process.header.msg_type & 0xff00) >> 8;
            packet_aux.payload[1] = (UINT8)(packet_to_process.header.msg_type & 0x00ff);

            Push_Serial_Tx_FIFO_Packet(Build_Packet_Serial(packet_aux.payload, MSG_ACK));
          }

        break;

        case MSG_HEALTH_RETRIEVE:
          Health_Manager_Set_Data(packet_aux.payload);
          Push_Serial_Tx_FIFO_Packet(Build_Packet_Serial(packet_aux.payload, MSG_HEALTH_SEND));
        break;

        case MSG_ADC_CALIBRATION_SEND:
          CnfManager_Save_Data(packet_to_process.payload, MSG_ADC_CALIBRATION_SEND);

          //update node current ADC calibration parameters data
          if(CnfManager_Update_Node_Config(ADC_CONFIG_CALIBRATION_DATA) == OK)
          {

            //load ack payload which is the msgtype of the msg received and send ack
            packet_aux.payload[0] = (UINT8)(packet_to_process.header.msg_type & 0xff00) >> 8;
            packet_aux.payload[1] = (UINT8)(packet_to_process.header.msg_type & 0x00ff);

            Push_Serial_Tx_FIFO_Packet(Build_Packet_Serial(packet_aux.payload, MSG_ACK));
          }
          break;

        case MSG_ADC_CALIBRATION_RETRIEVE:
          CnfManager_Load_Data(packet_aux.payload, MSG_ADC_CALIBRATION_RETRIEVE);
          Push_Serial_Tx_FIFO_Packet(Build_Packet_Serial(packet_aux.payload, MSG_ADC_CALIBRATION_SEND));
          break;

        case MSG_RADIO_DATA_SEND:
          //save mote Radio configuration
          CnfManager_Save_Data(packet_to_process.payload, MSG_RADIO_DATA_SEND);

          //update node current identity data
          if(CnfManager_Update_Node_Config(RADIO_CONFIG_NODE_DATA) == OK)
          {
            //load ack payload which is the msgtype of the msg received and send ack
            packet_aux.payload[0] = (UINT8)(packet_to_process.header.msg_type & 0xff00) >> 8;
            packet_aux.payload[1] = (UINT8)(packet_to_process.header.msg_type & 0x00ff);

            Push_Serial_Tx_FIFO_Packet(Build_Packet_Serial(packet_aux.payload, MSG_ACK));
          }
        break;

        case MSG_RADIO_DATA_RETRIEVE:
          CnfManager_Load_Data(packet_aux.payload, MSG_RADIO_DATA_RETRIEVE);
          Push_Serial_Tx_FIFO_Packet(Build_Packet_Serial(packet_aux.payload, MSG_RADIO_DATA_SEND));
        break;

        //TODO: other packet cases

        case MSG_ACK:
          //receiving an ACK
        break;

        default:
          //TODO: unknown message
        break;
      }
    }
    else
    {
      //check conditions to discard the packet received
      //if packet received 0 leap node
      //if packet received 0 leap repeater

      /*
         TODO:
        1 - Radio send the packet received
        2 - check if message is for local mote (static table)
        3 - check if message is for local repeater (repeater table)
        4 - spread message to local repeaters
      */

      //testing step 1
        //fill radio packet [packet_length, origin, destination,ACK needed byte, payload]
        //store radio packet into Tx fifo to be sent

    }
  }
}

/******************************************************************************
void Packet_Manager_Process_Motor(void)
   Packet manager, decouples packet receiving source from packet data payload

  Pre condition:
    None

  Post condition:
    Process a data packet received from the following sources and priority
      - Serial packet
      - radio packet

  Params:
    None

  Return:
    None

*******************************************************************************/
void Packet_Manager_Process_Motor(void)
{
  switch( packet_Manager_Motor_State )
  {
    case PACKET_MANAGER_CHECK_FOR_SERIAL_RX_PACKETS:

      //check if serial fifo is empty
      if(Is_Serial_Rx_FIFO_Empty())
      {
        //serial fifo empty, check the next Rx peripheral
        packet_Manager_Motor_State = PACKET_MANAGER_CHECK_FOR_RADIO_RX_PACKETS;
      }
      else
      {
        //serial fifo is not empty, get packet and process it
        Process_Packet(Get_Serial_Rx_FIFO_Packet());

        //check for Rx radio packets if a any
        packet_Manager_Motor_State = PACKET_MANAGER_CHECK_FOR_RADIO_RX_PACKETS;
      }
    break;

    case PACKET_MANAGER_CHECK_FOR_RADIO_RX_PACKETS:

      //TODO:
      packet_Manager_Motor_State = PACKET_MANAGER_CHECK_FOR_SERIAL_RX_PACKETS;

    break;
  }
}

