/*
 * error_report_manager.h
 *
 *  Created on: 11 abr. 2020
 *      Author: MAX PC
 */

#ifndef SUPERVISOR_SUPERVISOR_H_
#define SUPERVISOR_SUPERVISOR_H_

#include "globals.h"
#include "fsl_port.h"
#include "fsl_gpio.h"

enum
{
  FLASH_STOP_ERROR = 0,
  SPI_STOP_ERROR,
};


void Init_Supervisor(void);
void SignalStopError(UINT8 stopError_Code);

#endif /* SUPERVISOR_SUPERVISOR_H_ */
