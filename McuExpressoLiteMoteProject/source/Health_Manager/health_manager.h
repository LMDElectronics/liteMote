/*
 * health_manager.h
 *
 *  Created on: 13 may. 2020
 *      Author: MAX PC
 */

#ifndef HEALTH_MANAGER_HEALTH_MANAGER_H_
#define HEALTH_MANAGER_HEALTH_MANAGER_H_

#include <MCU_Drivers/adc/adc.h>
#include <MCU_Drivers/lptmr1_timer/lptmr1.h>
#include "globals.h"

enum
{
  IDLE_STATE = 0,
  SEND_TIMED_HEALTH_MESSAGE,
};

void Health_Manager_Init(void);

void Health_Manager_Motor(void);

UINT16 Health_Manager_Get_Bat_Voltage(void);
UINT16 Health_Manager_Get_Temperature(void);
UINT16 Health_Manager_Get_Voltage_Ref(void);

void Health_Manager_Set_Data(UINT8 *bufferToFill);

void Health_Manager_Reload_Timer(UINT16 timerCompValue);

#endif /* HEALTH_MANAGER_HEALTH_MANAGER_H_ */
