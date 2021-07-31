/*
 * serial_timers_hw_conf.h
 *
 *  Created on: 12 feb. 2019
 *      Author: MAX PC
 */

#ifndef SERIAL_MANAGER_SERIAL_TIMERS_HW_CONF_H_
#define SERIAL_MANAGER_SERIAL_TIMERS_HW_CONF_H_

#include "globals.h"
#include "typedef.h"
#include "fsl_pit.h"

UINT8 Serial_Timeout_Rx_Timer_Init(UINT32 baudRate);

void Start_Comm_Timeout_Timer(void);
void Stop_Comm_Timeout_Timer(void);

UINT8 Get_Timeout_Flag(void);
void Clear_Timeout_Flag(void);

#endif /* SERIAL_MANAGER_SERIAL_TIMERS_HW_CONF_H_ */
