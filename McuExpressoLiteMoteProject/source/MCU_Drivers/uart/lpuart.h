/*
 * uart_hw_conf.h
 *
 *  Created on: 1 feb. 2019
 *      Author: MAX PC
 */

#ifndef SERIAL_MANAGER_UART_HW_CONF_H_
#define SERIAL_MANAGER_UART_HW_CONF_H_

#include "pin_mux.h"
#include "clock_config.h"
#include "MKL82Z7.h"
#include "globals.h"
#include "fsl_lpuart.h"
#include "fsl_port.h"
#include "board.h"

#define MAX_SERIAL_DATA_BUFFER_SIZE 128

typedef struct str_serial_data
{
  volatile UINT8 *pSerialDataBuffer;
  volatile UINT8 serialDataBuffer_Elements;
}str_serial_data;

UINT8 Uart_HW_Init(void);
UINT8 Get_UART_Byte_Received_Flag(void);
void Clear_UART_Byte_Received_Flag(void);
UINT8 Get_Byte_RX_From_Comm_UART(void);
UINT32 Get_Comm_UART_Hw_BaudRate(void);

void Send_UART_Frame(UINT8 *data_To_Transfer, UINT8 data_To_Transfer_Lenght);
UINT8 Get_Frame_Sending_Flag(void);

str_serial_data UART_Get_serial_data_Rx_struct(void);
void UART_Clear_Rx_Buffer(void);

//UART TEST
void Send_byte_per_second(void);

#endif /* SERIAL_MANAGER_UART_HW_CONF_H_ */
