/*
 * Radio_Packet_FIFOs.c
 *
 *  Created on: 2 ago. 2021
 *      Author: MAX PC
 */

#include <RadioManager/Radio_Packet_FIFOs.h>
#include "string.h"
#include "globals.h"

//RX serial packet FIFO
UINT8 stored_radio_Rx_packets_index = 0;
UINT8 index_Rx_Radio_FIFO_Tail = 0;
UINT8 index_Rx_Radio_FIFO_Head = 0;
TR_packet packet_Radio_Rx_FIFO[MAX_RADIO_PACKETS_ALLOWED];

//TX serial packet FIFO
UINT8 stored_radio_Tx_packets_index = 0;
UINT8 index_Radio_Tx_FIFO_Tail = 0;
UINT8 index_Radio_Tx_FIFO_Head = 0;
TR_packet packet_Radio_Tx_FIFO[MAX_RADIO_PACKETS_ALLOWED];

/*******************************************************************************
  RX Radio FIFO FUNCTIONS
*******************************************************************************/

/*******************************************************************************
  void Radio_packet_Rx_FIFO_Init(void) function

  Pre condition:
    None

  Post condition:
    Intis the radio packet FIFO to 0's

  Params:
    None

  Returns:
    None

********************************************************************************/
void Radio_Packet_Rx_FIFO_Init(void)
{
  UINT8 i=0;

  stored_radio_Rx_packets_index = 0;
  index_Rx_Radio_FIFO_Tail = 0;
  index_Rx_Radio_FIFO_Head = 0;

  for(i=0; i<MAX_RADIO_PACKETS_ALLOWED; i++)
  {
    memset(&packet_Radio_Rx_FIFO[i].header, sizeof(packet_Radio_Rx_FIFO[i].header), 0);
    memset(packet_Radio_Rx_FIFO[i].payload, sizeof(packet_Radio_Rx_FIFO[i].payload), 0);
  }
}

/*******************************************************************************
  UINT8 Is_Radio_Rx_FIFO_Empty(void) function

  Pre condition:
    None

  Post condition:
    None

  Params:
    None

  Returns:
    returns TRUE if FIFO is empty, FALSE, while not empty

********************************************************************************/
UINT8 Is_Radio_Rx_FIFO_Empty(void)
{
  if( stored_radio_Rx_packets_index == 0 )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*******************************************************************************
  TS_packet Get_Radio_Rx_FIFO_Packet(void) function

  Pre condition:
    None

  Post condition:
    Returns a radio packet stored

  Params:
    None

  Returns:
    Radio packet if any, if no packet present, returns a 0 packet

********************************************************************************/
TR_packet Get_Radio_Rx_FIFO_Packet(void)
{
  TR_packet radio_packet;
  UINT8 i=0;

  ENTER_ATOMIC();

    //only get packet if at least one serial packet is stored
    if(stored_radio_Rx_packets_index > 0)
    {
      radio_packet.header.origin_node = packet_Radio_Rx_FIFO[index_Rx_Radio_FIFO_Tail].header.origin_node;
      radio_packet.header.destination_node = packet_Radio_Rx_FIFO[index_Rx_Radio_FIFO_Tail].header.destination_node;
      radio_packet.header.send_time = packet_Radio_Rx_FIFO[index_Rx_Radio_FIFO_Tail].header.send_time;
      radio_packet.header.ackNeeded = packet_Radio_Rx_FIFO[index_Rx_Radio_FIFO_Tail].header.ackNeeded;
      radio_packet.header.frame_payload_length = packet_Radio_Rx_FIFO[index_Rx_Radio_FIFO_Tail].header.frame_payload_length;

      for(i=0; i<radio_packet.header.frame_payload_length; i++)
      {
        radio_packet.payload[i] = packet_Radio_Rx_FIFO[index_Rx_Radio_FIFO_Tail].payload[i];
      }

      //p_To_Packet_Tail pointing to next packet to be delivered if any
      stored_radio_Rx_packets_index--;
      index_Rx_Radio_FIFO_Tail++;

      if(index_Rx_Radio_FIFO_Tail == MAX_RADIO_PACKETS_ALLOWED)
      {
        index_Rx_Radio_FIFO_Tail = 0;
      }
    }
    else
    {
      //returning 0 packet
      memset(&radio_packet.header, sizeof(radio_packet.header), 0);
      memset(radio_packet.payload, sizeof(radio_packet.payload), 0);
    }

  LEAVE_ATOMIC();

  return radio_packet;
}

/*******************************************************************************
  void Push_Radio_Rx_FIFO_Packet(UINT8 *dataBuffer) function

  Pre condition:
    None

  Post condition:
    stores a packet from a received byte array

  Params:
    dataBuffer, byte array

  Returns:
    None, pushes the radio data received into a packet

********************************************************************************/
void Push_Radio_Rx_FIFO_Packet(TR_packet radio_packet_To_Push)
{
  UINT8 i=0;

  ENTER_ATOMIC();

    packet_Radio_Rx_FIFO[index_Rx_Radio_FIFO_Head].header.origin_node = radio_packet_To_Push.header.origin_node;
    packet_Radio_Rx_FIFO[index_Rx_Radio_FIFO_Head].header.destination_node = radio_packet_To_Push.header.destination_node;
    packet_Radio_Rx_FIFO[index_Rx_Radio_FIFO_Head].header.send_time = radio_packet_To_Push.header.send_time;
    packet_Radio_Rx_FIFO[index_Rx_Radio_FIFO_Head].header.ackNeeded = radio_packet_To_Push.header.ackNeeded;
    packet_Radio_Rx_FIFO[index_Rx_Radio_FIFO_Head].header.frame_payload_length = radio_packet_To_Push.header.frame_payload_length;

    for(i=0; i<packet_Radio_Rx_FIFO[index_Rx_Radio_FIFO_Head].header.frame_payload_length; i++)
    {
      packet_Radio_Rx_FIFO[index_Rx_Radio_FIFO_Head].payload[i] = radio_packet_To_Push.payload[i];
    }

    //updating pointers
    stored_radio_Rx_packets_index++;
    index_Rx_Radio_FIFO_Head++;

    if(index_Rx_Radio_FIFO_Head == MAX_RADIO_PACKETS_ALLOWED)
    {
      index_Rx_Radio_FIFO_Head=0;
    }

    i=0;

  LEAVE_ATOMIC();
}

/*******************************************************************************
  TX RADIO FIFO FUNCTIONS
*******************************************************************************/
/*******************************************************************************
  void Radio_packet_Tx_FIFO_Init(void) function

  Pre condition:
    None

  Post condition:
    Intis the radio packet FIFO to 0's

  Params:
    None

  Returns:
    None

********************************************************************************/
void Radio_Packet_Tx_FIFO_Init(void)
{
  UINT8 i=0;

  stored_radio_Tx_packets_index = 0;
  index_Radio_Tx_FIFO_Tail = 0;
  index_Radio_Tx_FIFO_Head = 0;

  for(i=0; i<MAX_RADIO_PACKETS_ALLOWED; i++)
  {
    memset(&packet_Radio_Tx_FIFO[i].header, sizeof(packet_Radio_Tx_FIFO[i].header), 0);
    memset(packet_Radio_Tx_FIFO[i].payload, sizeof(packet_Radio_Tx_FIFO[i].payload), 0);
  }
}

/*******************************************************************************
  UINT8 Is_Radio_Tx_FIFO_Empty(void) function

  Pre condition:
    None

  Post condition:
    None

  Params:
    None

  Returns:
    returns TRUE if FIFO is empty, FALSE, while not empty

********************************************************************************/
UINT8 Is_Radio_Tx_FIFO_Empty(void)
{
  if( stored_radio_Tx_packets_index == 0 )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*******************************************************************************
  TS_packet Get_Radio_Tx_FIFO_Packet(void) function

  Pre condition:
    None

  Post condition:
    Returns a serial packet stored

  Params:
    None

  Returns:
    Radio packet if any, if no packet present, returns a 0 packet

********************************************************************************/
TR_packet Get_Radio_Tx_FIFO_Packet(void)
{
  TR_packet radio_packet;
  UINT8 i=0;

  ENTER_ATOMIC();

    //only get packet if at least one serial packet is stored
    if(stored_radio_Tx_packets_index > 0)
    {
      radio_packet.header.origin_node = packet_Radio_Tx_FIFO[index_Radio_Tx_FIFO_Tail].header.origin_node;
      radio_packet.header.destination_node = packet_Radio_Tx_FIFO[index_Radio_Tx_FIFO_Tail].header.destination_node;
      radio_packet.header.send_time = packet_Radio_Tx_FIFO[index_Radio_Tx_FIFO_Tail].header.send_time;
      radio_packet.header.ackNeeded = packet_Radio_Tx_FIFO[index_Radio_Tx_FIFO_Tail].header.ackNeeded;
      radio_packet.header.frame_payload_length = packet_Radio_Tx_FIFO[index_Radio_Tx_FIFO_Tail].header.frame_payload_length;

      for(i=0; i<radio_packet.header.frame_payload_length; i++)
      {
        radio_packet.payload[i] = packet_Radio_Tx_FIFO[index_Radio_Tx_FIFO_Tail].payload[i];
      }

      //p_To_Packet_Tail pointing to next packet to be delivered if any
      stored_radio_Tx_packets_index--;
      index_Radio_Tx_FIFO_Tail++;

      if(index_Radio_Tx_FIFO_Tail == MAX_RADIO_PACKETS_ALLOWED)
      {
        index_Radio_Tx_FIFO_Tail = 0;
      }
    }
    else
    {
      //returning 0 packet
      memset(&radio_packet.header, sizeof(radio_packet.header), 0);
      memset(radio_packet.payload, sizeof(radio_packet.payload), 0);
    }

  LEAVE_ATOMIC();

  return radio_packet;
}

/*******************************************************************************
  void Push_Radio_Rx_FIFO_Packet(UINT8 *dataBuffer) function

  Pre condition:
    None

  Post condition:
    stores a packet from a received byte array

  Params:
    dataBuffer, byte array

  Returns:
    None, pushes the radio data received into a packet

********************************************************************************/
void Push_Radio_Tx_FIFO_Packet(TR_packet radio_packet_To_Push)
{
  UINT8 i=0;

  ENTER_ATOMIC();

    packet_Radio_Tx_FIFO[index_Radio_Tx_FIFO_Head].header.origin_node = radio_packet_To_Push.header.origin_node;
    packet_Radio_Tx_FIFO[index_Radio_Tx_FIFO_Head].header.destination_node = radio_packet_To_Push.header.destination_node;
    packet_Radio_Tx_FIFO[index_Radio_Tx_FIFO_Head].header.send_time = radio_packet_To_Push.header.send_time;
    packet_Radio_Tx_FIFO[index_Radio_Tx_FIFO_Head].header.ackNeeded = radio_packet_To_Push.header.ackNeeded;
    packet_Radio_Tx_FIFO[index_Radio_Tx_FIFO_Head].header.frame_payload_length = radio_packet_To_Push.header.frame_payload_length;

    for(i=0; i<packet_Radio_Tx_FIFO[index_Radio_Tx_FIFO_Head].header.frame_payload_length; i++)
    {
      packet_Radio_Tx_FIFO[index_Radio_Tx_FIFO_Head].payload[i] = radio_packet_To_Push.payload[i];
    }

    //updating pointers
    stored_radio_Tx_packets_index++;
    index_Radio_Tx_FIFO_Head++;

    if(index_Radio_Tx_FIFO_Head == MAX_RADIO_PACKETS_ALLOWED)
    {
      index_Radio_Tx_FIFO_Head=0;
    }

    i=0;

  LEAVE_ATOMIC();
}



