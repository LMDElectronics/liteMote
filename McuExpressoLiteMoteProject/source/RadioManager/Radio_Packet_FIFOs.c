/*
 * Radio_Packet_FIFOs.c
 *
 *  Created on: 2 ago. 2021
 *      Author: MAX PC
 */

#include <RadioManager/Radio_Packet_FIFOs.h>

//RX serial packet FIFO
UINT8 stored_radio_Rx_packets_index = 0;
UINT8 index_Rx_Radio_FIFO_Tail = 0;
UINT8 index_Rx_Radio_FIFO_Head = 0;
//TS_packet packet_Rx_FIFO[MAX_PACKETS_ALLOWED];

//TX serial packet FIFO
UINT8 stored_radio_Tx_packets_index = 0;
UINT8 index_Radio_Tx_FIFO_Tail = 0;
UINT8 index_Radio_Tx_FIFO_Head = 0;
//TS_packet packet_Tx_FIFO[MAX_PACKETS_ALLOWED];



