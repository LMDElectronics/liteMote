/*
 * spi.h
 *
 *  Created on: 4 oct. 2019
 *      Author: MAX PC
 */

#include "clock_config.h"
#include "MKL82Z7.h"
#include "globals.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_clock.h"
#include "fsl_common.h"

#define CTAR_0  0
#define CTAR_1  1

#define TRANSFER_BAUDRATE 8000000

#ifndef EXTERNALFLASHMANAGER_SPI_SPI_H_
#define EXTERNALFLASHMANAGER_SPI_SPI_H_

#define NO_DEVICE           0
#define CS_EXTERNAL_FLASH   1
#define CS_S2LP_RADIO       2

#define TRANSFER_SIZE 256U

void SPI_System_Init(void);
UINT32 SPI_GetClockFrequency(SPI_Type *spiPeripheralNumber);
UINT32 SPI_GetStatusFlag(SPI_Type *spiPeripheralNumber);

uint8_t SPI_Send_And_Receive_Byte_Blocking(SPI_Type *spiPeripheralNumber, uint8_t byte);
void SPI_Send_NonBlocking(SPI_Type *spiPeripheralNumber, uint8_t *dataBytes, uint32_t dataLength);

//dealing with CS
void Enable_CS(uint8_t device_CS_to_enable);
void Disable_CS(uint8_t device_CS_to_disable);

#endif /* EXTERNALFLASHMANAGER_SPI_SPI_H_ */
