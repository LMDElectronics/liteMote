/*
 * packet_radio_conf.h
 *
 *  Created on: 20 jun. 2020
 *      Author: MAX PC
 */

#ifndef PACKET_MANAGER_PACKET_RADIO_CONF_PACKET_RADIO_CONF_H_
#define PACKET_MANAGER_PACKET_RADIO_CONF_PACKET_RADIO_CONF_H_

#include "globals.h"

#define MSG_RADIO_CONF_SEND_LENGTH         sizeof(TMote_Radio_Conf_Data)
#define MSG_RADIO_CONF_RETRIEVE_LENGTH     0

#define EURO_FREQ_CENTER        0
#define AMERICAN_FREQ_CENTER    1

#define INDEXED_2FSK        0
#define INDEXED_4FSK        1
#define INDEXED_2GFSK_BT_1  2
#define INDEXED_4GFSK_BT_1  3
#define INDEXED_ASK_OOK     4
#define INDEXED_2GFSK_BT_05 5
#define INDEXED_4GFSK_BT_05 6

#define INDEXED_DATA_RATE_25_KBPS  0
#define INDEXED_DATA_RATE_50_KBPS  1
#define INDEXED_DATA_RATE_100_KBPS  2
#define INDEXED_DATA_RATE_200_KBPS  3
#define INDEXED_DATA_RATE_300_KBPS  4
#define INDEXED_DATA_RATE_400_KBPS  5
#define INDEXED_DATA_RATE_500_KBPS  6

typedef struct TMote_Radio_Conf_Data
{
  UINT8 freq_Band;  //Mhz
  UINT8 modulation; //modulation type
  UINT8 dataRate;  //KBps
  UINT8 channel;    //radio channel in selected band
  UINT8 power;     //Tx radio power

}TMote_Radio_Conf_Data;

#endif /* PACKET_MANAGER_PACKET_RADIO_CONF_PACKET_RADIO_CONF_H_ */
