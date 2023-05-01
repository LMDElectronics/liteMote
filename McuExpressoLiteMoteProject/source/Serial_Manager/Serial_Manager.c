/*
 * Serial_Manager.c
 *
 *  Created on: 1 feb. 2019
 *      Author: MAX PC
 */

#include <Serial_Manager/base64_codec.h>
#include <Serial_Manager/Serial_Manager.h>
#include <CRC_tools/crc_tools.h>
#include <MCU_Drivers/pit_timer/pit.h>
#include <MCU_Drivers/uart/lpuart.h>

UINT8 serial_Manager_Motor_State = SERIAL_MANAGER_WAIT_ENCODED_START_FRAME;
UINT8 serial_Manager_Tx_State = SERIAL_MANAGER_TX_CHECK_TO_SEND;

UINT8 receiving_Serial_Frame_Flag = FALSE;
UINT8 serial_Rx_byte_timeout_flag = FALSE;

UINT8 serial_Frame_Rx_Index = 0;
UINT8 serial_encoded_Frame_RX_Buffer[MAX_SERIAL_FRAME_ENCODED_SIZE];

UINT8 serial_frame_RX_decoded_length = 0;
UINT8 serial_decodedFrame_RX_Buffer[MAX_PACKET_SIZE];

UINT8 serial_Frame_Tx_decoded_Index = 0;
UINT8 serial_Tx_decoded_Frame[MAX_PACKET_SIZE];
UINT16 serial_crc_frame = 0;

UINT8 serial_Frame_Tx_encoded_Index = 0;
UINT8 serial_Tx_encoded_Frame[MAX_SERIAL_FRAME_ENCODED_SIZE];
UINT16 serial_tx_crc = 0;

/******************************************************************************
  UINT8 Serial_Manager_Init(void)
   Serial Manager Init function

  Pre condition:
    None

  Post condition:
    Inits the Serial Communications Manager Module

  Params:
    None

  Return:
    INIT_OK, if initalization succesful
    INIT_FAILED, if initialization failed

*******************************************************************************/
UINT8 Serial_Manager_Init(void)
{
  UINT8 retVal = INIT_FAILED;

  //Initizalizing HW UART
  if( Uart_HW_Init() == INIT_FAILED )
  {
    retVal = INIT_FAILED;
  }
  else
  {
    //Inits the serial packet FIFO
    Serial_Packet_Rx_FIFO_Init();

    //Inits the Serial Manager Motor variables
    receiving_Serial_Frame_Flag = FALSE;
    serial_Frame_Rx_Index = 0;

    Clear_Timeout_Flag();

    serial_Manager_Motor_State = SERIAL_MANAGER_WAIT_ENCODED_START_FRAME;
    serial_Manager_Tx_State = SERIAL_MANAGER_TX_CHECK_TO_SEND;

    //Inits the PIT timer channel 0 for Rx timeout generation
    Serial_Timeout_Rx_Timer_Init(Get_Comm_UART_Hw_BaudRate());

    retVal = INIT_OK;
  }

  return retVal;
}

/******************************************************************************
void Serial_Manager_Flush_Encoded_Frame(void)
   Serial Manager encoded frame flush

  Pre condition:
    Serial encoded frame is being received

  Post condition:
    sets to 0 the encoded serial frame buffer index

  Params:
    None

  Return:
    None
*******************************************************************************/
void Serial_Manager_Flush_Encoded_Frame(void)
{
  serial_Frame_Rx_Index = 0;
}

/******************************************************************************
  TS_packet From_Serial_Frame_To_Packet(UINT8 dataBuffer)
   builds a packet from a serial decoded frame

  Pre condition:
    None

  Post condition:
    None

  Params:
    UINT8 serial raw decoded frame

  Return:
    serial packet TS_packet

*******************************************************************************/
TS_packet From_Serial_Frame_To_Packet(UINT8 *dataBuffer)
{
  UINT8 i=0;
  TS_packet serial_rx_packet;

  //marshalling
  serial_rx_packet.header.origin_node = dataBuffer[DATA_BUFFER_ORIGIN_NODE_OFFSET];
  serial_rx_packet.header.origin_node <<= 8;
  serial_rx_packet.header.origin_node |= dataBuffer[DATA_BUFFER_ORIGIN_NODE_OFFSET + 1];

  serial_rx_packet.header.destination_node = dataBuffer[DATA_BUFFER_DESTINATION_NODE_OFFSET];
  serial_rx_packet.header.destination_node <<= 8;
  serial_rx_packet.header.destination_node |= dataBuffer[DATA_BUFFER_DESTINATION_NODE_OFFSET + 1];

  serial_rx_packet.header.send_time = dataBuffer[DATA_BUFFER_SEND_TIME_OFFSET];
  serial_rx_packet.header.send_time <<= 8;
  serial_rx_packet.header.send_time |= dataBuffer[DATA_BUFFER_SEND_TIME_OFFSET + 1];

  serial_rx_packet.header.msg_type = dataBuffer[DATA_BUFFER_MSG_TYPE_OFFSET];
  serial_rx_packet.header.msg_type <<= 8;
  serial_rx_packet.header.msg_type |= dataBuffer[DATA_BUFFER_MSG_TYPE_OFFSET + 1];

  serial_rx_packet.header.frame_payload_length = dataBuffer[DATA_BUFFER_PAYLOAD_LENGTH_OFFSET];

  for(i=0; i<serial_rx_packet.header.frame_payload_length; i++)
  {
    serial_rx_packet.payload[i] = dataBuffer[DATA_BUFFER_PAYLOAD_START_OFFSET + i];
  }

  return serial_rx_packet;
}

/******************************************************************************
UINT8 From_Packet_To_Serial_Frame(TS_packet serial_tx_packet, UINT8 *dataBuffer)
   builds a frame from a serial packet

  Pre condition:
    None

  Post condition:
    None

  Params:
    serial packet to tx, UINT8 serial raw decoded frame

  Return:
    number of bytes forming the frame

*******************************************************************************/
UINT8 From_Packet_To_Serial_Frame(TS_packet serial_tx_packet, UINT8 *dataBuffer)
{
  UINT8 i=0;
  UINT8 length = 0;

  //marshalling
  dataBuffer[DATA_BUFFER_ORIGIN_NODE_OFFSET] = (UINT8)((serial_tx_packet.header.origin_node & (0xFF00)) >> 8); length++;
  dataBuffer[DATA_BUFFER_ORIGIN_NODE_OFFSET + 1] = (UINT8)(serial_tx_packet.header.origin_node & (0x00FF)); length++;

  dataBuffer[DATA_BUFFER_DESTINATION_NODE_OFFSET] = (UINT8)((serial_tx_packet.header.destination_node & (0xFF00)) >> 8); length++;
  dataBuffer[DATA_BUFFER_DESTINATION_NODE_OFFSET + 1] = (UINT8)(serial_tx_packet.header.destination_node & (0x00FF)); length++;

  dataBuffer[DATA_BUFFER_SEND_TIME_OFFSET] = (UINT8)((serial_tx_packet.header.send_time & (0xFF00)) >> 8); length++;
  dataBuffer[DATA_BUFFER_SEND_TIME_OFFSET + 1] = (UINT8)(serial_tx_packet.header.send_time & (0x00FF)); length++;

  dataBuffer[DATA_BUFFER_MSG_TYPE_OFFSET] = (UINT8)((serial_tx_packet.header.msg_type & (0xFF00)) >> 8); length++;
  dataBuffer[DATA_BUFFER_MSG_TYPE_OFFSET + 1] = (UINT8)(serial_tx_packet.header.msg_type & (0x00FF)); length++;

  dataBuffer[DATA_BUFFER_PAYLOAD_LENGTH_OFFSET] = serial_tx_packet.header.frame_payload_length; length++;

  for(i=0; i<serial_tx_packet.header.frame_payload_length; i++)
  {
    dataBuffer[DATA_BUFFER_PAYLOAD_START_OFFSET + i] = serial_tx_packet.payload[i];
    length++;
  }

  return length;
}

/******************************************************************************
  void Serial_Manager_Motor(void)
   Serial Manager Motor

  Pre condition:
    Serial_Manager_Init() should have been called before

  Post condition:
    Runs the Serial Manager task

  Params:
    None

  Return:
    None
*******************************************************************************/
void Serial_Manager_RX_Motor(void)
{
  switch(serial_Manager_Motor_State)
  {

    /*
      encoded serial frame start detection state
    */
    case SERIAL_MANAGER_WAIT_ENCODED_START_FRAME:

      if(Get_UART_Byte_Received_Flag() == FALSE)
      {
        //No start frame byte received
        serial_Manager_Motor_State = SERIAL_MANAGER_WAIT_ENCODED_START_FRAME;
      }
      else
      {
        if(receiving_Serial_Frame_Flag == TRUE)
        {
          /*
            start frame byte received while serial frame bytes are
            still being received, aborting.
          */
          receiving_Serial_Frame_Flag = FALSE;

          Serial_Manager_Flush_Encoded_Frame();
          serial_Manager_Motor_State = SERIAL_MANAGER_WAIT_ENCODED_START_FRAME;
        }
        else
        {
          if(Get_Byte_RX_From_Comm_UART() == BASE64_FRAME_START_BYTE)
          {

            //start timeout timer
            Start_Comm_Timeout_Timer();

            //serial encoded frame RX start byte detected, flag it.
            receiving_Serial_Frame_Flag = TRUE;
            serial_Manager_Motor_State = SERIAL_MANAGER_GET_ENCODED_FRAME;
          }
        }

        Clear_UART_Byte_Received_Flag();
      }
    break;

    /*
      encoded serial frame get payload
    */
    case SERIAL_MANAGER_GET_ENCODED_FRAME:

      if(receiving_Serial_Frame_Flag == FALSE)
      {

        //stop timeout timer
        Stop_Comm_Timeout_Timer();

        //not receiving a frame, aborting Rx
        receiving_Serial_Frame_Flag = FALSE;

        Serial_Manager_Flush_Encoded_Frame();
        serial_Manager_Motor_State = SERIAL_MANAGER_WAIT_ENCODED_START_FRAME;
      }
      else
      {
        if(Get_UART_Byte_Received_Flag() == FALSE)
        {
          //check for byte timeout
          if(Get_Timeout_Flag() == TRUE)
          {
            //stop timeout timer
            Stop_Comm_Timeout_Timer();

            //timeout reached, aborting Rx
            Clear_Timeout_Flag();

            Serial_Manager_Flush_Encoded_Frame();
            serial_Manager_Motor_State = SERIAL_MANAGER_WAIT_ENCODED_START_FRAME;
          }
        }
        else
        {
          //receiving a new data byte
          if(Get_Byte_RX_From_Comm_UART() == BASE64_FRAME_START_BYTE)
          {

            //received start byte while receiving a frame. aborting
            receiving_Serial_Frame_Flag = FALSE;

            Serial_Manager_Flush_Encoded_Frame();
            serial_Manager_Motor_State = SERIAL_MANAGER_WAIT_ENCODED_START_FRAME;
          }
          else
          {
            if(Get_Byte_RX_From_Comm_UART() == BASE64_FRAME_STOP_BYTE)
            {
              //stop timeout timer
              Stop_Comm_Timeout_Timer();

              //stop byte found, signal frame end, get ready to decode the frame
              receiving_Serial_Frame_Flag = FALSE;

              serial_Manager_Motor_State = SERIAL_MANAGER_DECODE_SERIAL_FRAME;
            }
            else
            {
              if(serial_Frame_Rx_Index > MAX_SERIAL_FRAME_ENCODED_SIZE)
              {
                //stop timeout timer
                Stop_Comm_Timeout_Timer();

                //serial encoded frame  too long, suspicious, abort
                receiving_Serial_Frame_Flag = FALSE;

                Serial_Manager_Flush_Encoded_Frame();
                serial_Manager_Motor_State = SERIAL_MANAGER_WAIT_ENCODED_START_FRAME;
              }
              else
              {
                //stop timeout timer
                Stop_Comm_Timeout_Timer();

                //new frame byte received, buffer it.
                serial_encoded_Frame_RX_Buffer[serial_Frame_Rx_Index++] = Get_Byte_RX_From_Comm_UART();

                //keep in the same state
                serial_Manager_Motor_State = SERIAL_MANAGER_GET_ENCODED_FRAME;
              }
            }
          }

          Clear_UART_Byte_Received_Flag();
        }
      }
    break;

    case SERIAL_MANAGER_DECODE_SERIAL_FRAME:
      if(receiving_Serial_Frame_Flag == TRUE)
      {
        //stop timeout timer
        Stop_Comm_Timeout_Timer();

        //receiving frame is TRUE, jump to receiving state
        serial_Manager_Motor_State = SERIAL_MANAGER_GET_ENCODED_FRAME;
      }
      else
      {
        //stop timeout timer
        Stop_Comm_Timeout_Timer();

        //calculate frame decoded size
        serial_frame_RX_decoded_length = B64DecodedLength((const char *)&serial_encoded_Frame_RX_Buffer[0], serial_Frame_Rx_Index);

        //decoding frame received
        B64Decode((const char *)&serial_encoded_Frame_RX_Buffer[0], serial_Frame_Rx_Index, (char *)serial_decodedFrame_RX_Buffer, serial_frame_RX_decoded_length);

        //extract crc from decoded frame
        serial_crc_frame = serial_decodedFrame_RX_Buffer[serial_frame_RX_decoded_length-2];
        serial_crc_frame  <<= 8;
        serial_crc_frame |= serial_decodedFrame_RX_Buffer[serial_frame_RX_decoded_length-1];

        if(serial_crc_frame != Crc16Calc(0, serial_decodedFrame_RX_Buffer, serial_frame_RX_decoded_length-2))
        {
          //CRC doesnt match, suspicious frame, abort
          receiving_Serial_Frame_Flag = FALSE;

          Serial_Manager_Flush_Encoded_Frame();
          serial_Manager_Motor_State = SERIAL_MANAGER_WAIT_ENCODED_START_FRAME;
        }
        else
        {
          //push serial frame into serial frame FIFO
          Push_Serial_Rx_FIFO_Packet(From_Serial_Frame_To_Packet(serial_decodedFrame_RX_Buffer));

          //flush for the next frame
          receiving_Serial_Frame_Flag = FALSE;

          Serial_Manager_Flush_Encoded_Frame();
          serial_Manager_Motor_State = SERIAL_MANAGER_WAIT_ENCODED_START_FRAME;
        }
      }

    break;

    default:

    break;
  }
}

/******************************************************************************
void Serial_Manager_Tx_Task(void)
   Serial Manager Tx Task

  Pre condition:

  Post condition:
    Encodes a serial packet ready to be sent

  Params:
    None

  Return:
    None
*******************************************************************************/
void Serial_Manager_Tx_Motor(void)
{

  switch(serial_Manager_Tx_State)
  {
    case SERIAL_MANAGER_TX_CHECK_TO_SEND:

      if(Is_Serial_Tx_FIFO_Empty())
      {
        //Tx FIFO is empty, nothing to serial send, take a rest
      }
      else
      {
        if(Get_Frame_Sending_Flag())
        {
          //busy sending a frame, system must wait until tx finishes
        }
        else
        {
          /*
            Tx FIFO not empty and no frame is being transmitted, send the packet:
              - Get a serial packet from Tx FIFO
              - Calc packet CRC
              - Encode serial packet
              - Send serial packet
           */

          //build raw frame from Tx FIFO packet
          serial_Frame_Tx_decoded_Index = From_Packet_To_Serial_Frame(Get_Serial_Tx_FIFO_Packet(), serial_Tx_decoded_Frame);

          //add crc
          serial_tx_crc = Crc16Calc(0, serial_Tx_decoded_Frame, serial_Frame_Tx_decoded_Index);
          serial_Tx_decoded_Frame[serial_Frame_Tx_decoded_Index] = (UINT8)((serial_tx_crc & (0xff00)) >> 8);
          serial_Frame_Tx_decoded_Index++;
          serial_Tx_decoded_Frame[serial_Frame_Tx_decoded_Index] = (UINT8)((serial_tx_crc & (0x00ff)));
          serial_Frame_Tx_decoded_Index++;

          //generate encoded payload length
          serial_Frame_Tx_encoded_Index = B64EncodedLength(serial_Frame_Tx_decoded_Index);

          //encode frame
          B64Encode((char *)serial_Tx_decoded_Frame,serial_Frame_Tx_decoded_Index,(char *)&serial_Tx_encoded_Frame[1],serial_Frame_Tx_encoded_Index);

          //sum and start bit length
          serial_Frame_Tx_encoded_Index = serial_Frame_Tx_encoded_Index + 1;

          //add start and stop bits
          serial_Tx_encoded_Frame[0] = BASE64_FRAME_START_BYTE;
          serial_Tx_encoded_Frame[serial_Frame_Tx_encoded_Index++] = BASE64_FRAME_STOP_BYTE;

          //start tx transfer
          Send_UART_Frame(serial_Tx_encoded_Frame, serial_Frame_Tx_encoded_Index);

          //clear index after send serial data
          serial_Frame_Tx_encoded_Index = 0;

          //move to wait for frame sent state
          serial_Manager_Tx_State = SERIAL_MANAGER_TX_WAIT_FOR_TX_COMPLETED;
        }
      }

    break;

    case SERIAL_MANAGER_TX_WAIT_FOR_TX_COMPLETED:

      if(Get_Frame_Sending_Flag())
      {
        //frame being transmitted, keep in this state
        serial_Manager_Tx_State = SERIAL_MANAGER_TX_WAIT_FOR_TX_COMPLETED;
      }
      else
      {
        //frame transmitted, clear flags and return for wait to send frames
        serial_Manager_Tx_State = SERIAL_MANAGER_TX_CHECK_TO_SEND;
      }

    break;

    default:
      //error occurred
    break;

  }
}

























