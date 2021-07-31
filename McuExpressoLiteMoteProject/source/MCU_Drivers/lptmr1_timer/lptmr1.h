/*
 * health.timer.h
 *
 *  Created on: 17 may. 2020
 *      Author: MAX PC
 */

#ifndef HEALTH_MANAGER_HEALTH_TIMER_HEALTH_TIMER_H_
#define HEALTH_MANAGER_HEALTH_TIMER_HEALTH_TIMER_H_

#include "globals.h"

#define MINUTES_TO_COUNT(minutes, clockFreqInHz,preescalerValue) (UINT32)((UINT32)((minutes * 60 * clockFreqInHz) / preescalerValue))

void Health_Timer_Init(void);
void Health_Timer_Load_Counter_CMP(UINT16 data);
void Health_Timer_Start(void);
void Health_Timer_Stop(void);

void Health_Timer_Test(void);

UINT8 Get_Tx_Health_Flag(void);
void Reset_Tx_Health_Flag(void);

#endif /* HEALTH_MANAGER_HEALTH_TIMER_HEALTH_TIMER_H_ */
