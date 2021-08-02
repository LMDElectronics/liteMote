/*
 * Serial_packet_Rx_FIFO.c
 *
 *  Created on: 1 feb. 2019
 *      Author: MAX PC
 */

#include <Serial_Manager/Serial_Packet_FIFOs.h>
#include "string.h"
#include "globals.h"

//RX serial packet FIFO
UINT8 stored_serial_Rx_packets_index = 0;
UINT8 index_Rx_Serial_FIFO_Tail = 0;
UINT8 index_Rx_Serial_FIFO_Head = 0;
TS_packet packet_Rx_FIFO[MAX_PACKETS_ALLOWED];

//TX serial packet FIFO
UINT8 stored_serial_Tx_packets_index = 0;
UINT8 index_Tx_Serial_FIFO_Tail = 0;
UINT8 index_Tx_Serial_FIFO_Head = 0;
TS_packet packet_Tx_FIFO[MAX_PACKETS_ALLOWED];

/*******************************************************************************
  RX SERIAL FIFO FUNCTIONS
*******************************************************************************/

/*******************************************************************************
  void Serial_packet_Rx_FIFO_Init(void) function

  Pre condition:
    None

  Post condition:
    Intis the serial packet FIFO to 0's

  Params:
    None

  Returns:
    None

********************************************************************************/
void Serial_Packet_Rx_FIFO_Init(void)
{
  UINT8 i=0;

  stored_serial_Rx_packets_index = 0;
  index_Rx_Serial_FIFO_Tail = 0;
  index_Rx_Serial_FIFO_Head = 0;

  for(i=0; i<MAX_PACKETS_ALLOWED; i++)
  {
    memset(&packet_Rx_FIFO[i].header, sizeof(packet_Rx_FIFO[i].header), 0);
    memset(packet_Rx_FIFO[i].payload, sizeof(packet_Rx_FIFO[i].payload), 0);
  }
}

/*******************************************************************************
  UINT8 Is_Serial_Rx_FIFO_Empty(void) function

  Pre condition:
    None

  Post condition:
    None

  Params:
    None

  Returns:
    returns TRUE if FIFO is empty, FALSE, while not empty

********************************************************************************/
UINT8 Is_Serial_Rx_FIFO_Empty(void)
{
  if( stored_serial_Rx_packets_index == 0 )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*******************************************************************************
  TS_packet Get_Serial_Rx_FIFO_Packet(void) function

  Pre condition:
    None

  Post condition:
    Returns a serial packet stored

  Params:
    None

  Returns:
    Serial packet if any, if no packet present, returns a 0 packet

********************************************************************************/
TS_packet Get_Serial_Rx_FIFO_Packet(void)
{
  TS_packet serial_packet;
  UINT8 i=0;

  ENTER_ATOMIC();

    //only get packet if at least one serial packet is stored
    if(stored_serial_Rx_packets_index > 0)
    {
      serial_packet.header.destination_node = packet_Rx_FIFO[index_Rx_Serial_FIFO_Tail].header.destination_node;
      serial_packet.header.msg_type = packet_Rx_FIFO[index_Rx_Serial_FIFO_Tail].header.msg_type;
      serial_packet.header.origin_node = packet_Rx_FIFO[index_Rx_Serial_FIFO_Tail].header.origin_node;
      serial_packet.header.send_time = packet_Rx_FIFO[index_Rx_Serial_FIFO_Tail].header.send_time;
      serial_packet.header.frame_payload_length = packet_Rx_FIFO[index_Rx_Serial_FIFO_Tail].header.frame_payload_length;

      for(i=0; i<serial_packet.header.frame_payload_length; i++)
      {
        serial_packet.payload[i] = packet_Rx_FIFO[index_Rx_Serial_FIFO_Tail].payload[i];
      }

      //p_To_Packet_Tail pointing to next packet to be delivered if any
      stored_serial_Rx_packets_index--;
      index_Rx_Serial_FIFO_Tail++;

      if(index_Rx_Serial_FIFO_Tail == MAX_PACKETS_ALLOWED)
      {
        index_Rx_Serial_FIFO_Tail = 0;
      }
    }
    else
    {
      //returning 0 packet
      memset(&serial_packet.header, sizeof(serial_packet.header), 0);
      memset(serial_packet.payload, sizeof(serial_packet.payload), 0);
    }

  LEAVE_ATOMIC();

  return serial_packet;
}

/*******************************************************************************
  void Push_Serial_Rx_FIFO_Packet(UINT8 *dataBuffer) function

  Pre condition:
    None

  Post condition:
    stores a packet from a received byte array

  Params:
    dataBuffer, byte array

  Returns:
    None, pushes the serial data received into a packet

********************************************************************************/
void Push_Serial_Rx_FIFO_Packet(TS_packet serial_packet_To_Push)
{
  UINT8 i=0;

  ENTER_ATOMIC();

    packet_Rx_FIFO[index_Rx_Serial_FIFO_Head].header.destination_node = serial_packet_To_Push.header.destination_node;
    packet_Rx_FIFO[index_Rx_Serial_FIFO_Head].header.origin_node = serial_packet_To_Push.header.origin_node;
    packet_Rx_FIFO[index_Rx_Serial_FIFO_Head].header.send_time = serial_packet_To_Push.header.send_time;
    packet_Rx_FIFO[index_Rx_Serial_FIFO_Head].header.msg_type = serial_packet_To_Push.header.msg_type;
    packet_Rx_FIFO[index_Rx_Serial_FIFO_Head].header.frame_payload_length = serial_packet_To_Push.header.frame_payload_length;

    for(i=0; i<packet_Rx_FIFO[index_Rx_Serial_FIFO_Head].header.frame_payload_length; i++)
    {
      packet_Rx_FIFO[index_Rx_Serial_FIFO_Head].payload[i] = serial_packet_To_Push.payload[i];
    }

    //updating pointers
    stored_serial_Rx_packets_index++;
    index_Rx_Serial_FIFO_Head++;

    if(index_Rx_Serial_FIFO_Head == MAX_PACKETS_ALLOWED)
    {
      index_Rx_Serial_FIFO_Head=0;
    }

    i=0;

  LEAVE_ATOMIC();
}

/*******************************************************************************
  TX SERIAL FIFO FUNCTIONS
*******************************************************************************/
/*******************************************************************************
  void Serial_packet_Tx_FIFO_Init(void) function

  Pre condition:
    None

  Post condition:
    Intis the serial packet FIFO to 0's

  Params:
    None

  Returns:
    None

********************************************************************************/
void Serial_Packet_Tx_FIFO_Init(void)
{
  UINT8 i=0;

  stored_serial_Tx_packets_index = 0;
  index_Tx_Serial_FIFO_Tail = 0;
  index_Tx_Serial_FIFO_Head = 0;

  for(i=0; i<MAX_PACKETS_ALLOWED; i++)
  {
    memset(&packet_Tx_FIFO[i].header, sizeof(packet_Tx_FIFO[i].header), 0);
    memset(packet_Tx_FIFO[i].payload, sizeof(packet_Tx_FIFO[i].payload), 0);
  }
}

/*******************************************************************************
  UINT8 Is_Serial_Tx_FIFO_Empty(void) function

  Pre condition:
    None

  Post condition:
    None

  Params:
    None

  Returns:
    returns TRUE if FIFO is empty, FALSE, while not empty

********************************************************************************/
UINT8 Is_Serial_Tx_FIFO_Empty(void)
{
  if( stored_serial_Tx_packets_index == 0 )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*******************************************************************************
  TS_packet Get_Serial_Tx_FIFO_Packet(void) function

  Pre condition:
    None

  Post condition:
    Returns a serial packet stored

  Params:
    None

  Returns:
    Serial packet if any, if no packet present, returns a 0 packet

********************************************************************************/
TS_packet Get_Serial_Tx_FIFO_Packet(void)
{
  TS_packet serial_packet;
  UINT8 i=0;

  ENTER_ATOMIC();

    //only get packet if at least one serial packet is stored
    if(stored_serial_Tx_packets_index > 0)
    {
      serial_packet.header.destination_node = packet_Tx_FIFO[index_Tx_Serial_FIFO_Tail].header.destination_node;
      serial_packet.header.msg_type = packet_Tx_FIFO[index_Tx_Serial_FIFO_Tail].header.msg_type;
      serial_packet.header.origin_node = packet_Tx_FIFO[index_Tx_Serial_FIFO_Tail].header.origin_node;
      serial_packet.header.send_time = packet_Tx_FIFO[index_Tx_Serial_FIFO_Tail].header.send_time;
      serial_packet.header.frame_payload_length = packet_Tx_FIFO[index_Tx_Serial_FIFO_Tail].header.frame_payload_length;

      for(i=0; i<serial_packet.header.frame_payload_length; i++)
      {
        serial_packet.payload[i] = packet_Tx_FIFO[index_Tx_Serial_FIFO_Tail].payload[i];
      }

      //p_To_Packet_Tail pointing to next packet to be delivered if any
      stored_serial_Tx_packets_index--;
      index_Tx_Serial_FIFO_Tail++;

      if(index_Tx_Serial_FIFO_Tail == MAX_PACKETS_ALLOWED)
      {
        index_Tx_Serial_FIFO_Tail = 0;
      }
    }
    else
    {
      //returning 0 packet
      memset(&serial_packet.header, sizeof(serial_packet.header), 0);
      memset(serial_packet.payload, sizeof(serial_packet.payload), 0);
    }

  LEAVE_ATOMIC();

  return serial_packet;
}

/*******************************************************************************
  void Push_Serial_Rx_FIFO_Packet(UINT8 *dataBuffer) function

  Pre condition:
    None

  Post condition:
    stores a packet from a received byte array

  Params:
    dataBuffer, byte array

  Returns:
    None, pushes the serial data received into a packet

********************************************************************************/
void Push_Serial_Tx_FIFO_Packet(TS_packet serial_packet_To_Push)
{
  UINT8 i=0;

  ENTER_ATOMIC();

    packet_Tx_FIFO[index_Tx_Serial_FIFO_Head].header.destination_node = serial_packet_To_Push.header.destination_node;
    packet_Tx_FIFO[index_Tx_Serial_FIFO_Head].header.origin_node = serial_packet_To_Push.header.origin_node;
    packet_Tx_FIFO[index_Tx_Serial_FIFO_Head].header.send_time = serial_packet_To_Push.header.send_time;
    packet_Tx_FIFO[index_Tx_Serial_FIFO_Head].header.msg_type = serial_packet_To_Push.header.msg_type;
    packet_Tx_FIFO[index_Tx_Serial_FIFO_Head].header.frame_payload_length = serial_packet_To_Push.header.frame_payload_length;

    for(i=0; i<packet_Tx_FIFO[index_Tx_Serial_FIFO_Head].header.frame_payload_length; i++)
    {
      packet_Tx_FIFO[index_Tx_Serial_FIFO_Head].payload[i] = serial_packet_To_Push.payload[i];
    }

    //updating pointers
    stored_serial_Tx_packets_index++;
    index_Tx_Serial_FIFO_Head++;

    if(index_Tx_Serial_FIFO_Head == MAX_PACKETS_ALLOWED)
    {
      index_Tx_Serial_FIFO_Head=0;
    }

    i=0;


  LEAVE_ATOMIC();
}

