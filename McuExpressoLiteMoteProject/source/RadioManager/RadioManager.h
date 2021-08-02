/*
 * RadioManager.h
 *
 *  Created on: 4 jun. 2020
 *      Author: MAX PC
 */

#ifndef RADIOMANAGER_RADIOMANAGER_H_
#define RADIOMANAGER_RADIOMANAGER_H_

#include "globals.h"
#include <RadioManager/Radio_Packet_FIFOs.h>
#include "packet_manager/packet_radio_conf/packet_radio_conf.h"

void Radio_Manager_Config(void);
void Radio_Manager_Init(void);
void Radio_Manager_Tx_Task(void);

#endif /* RADIOMANAGER_RADIOMANAGER_H_ */
