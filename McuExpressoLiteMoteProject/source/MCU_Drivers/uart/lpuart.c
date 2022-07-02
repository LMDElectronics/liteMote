/*
 * uart_hw_conf.c
 *
 *  Created on: 1 feb. 2019
 *      Author: MAX PC
 */

/******************************************************************************
  UART HW DEPENDANT FUNCTIONS
*******************************************************************************/
#include <MCU_Drivers/uart/lpuart.h>

volatile UINT8 serial_Data_Buffer[MAX_SERIAL_DATA_BUFFER_SIZE];
volatile UINT8 serial_Buffer_Index = 0;

volatile UINT8 byte_received_flag = FALSE;
volatile UINT8 byte_received = 0;

lpuart_config_t config;

//handle Tx non blocking transfers
lpuart_handle_t g_lpuartHandle;
lpuart_transfer_t xfer;
UINT8 serial_frame_sending_flag = FALSE;

volatile UINT8 bytes_to_Send = 0;
volatile UINT8 bytes_Sent = 0;

UINT8 serial_Tx_Data_Buffer[MAX_SERIAL_DATA_BUFFER_SIZE];

/******************************************************************************
  UINT8 Uart_HW_Init(void):
   Communications UART Initialization functions

  Pre condition:
    None

  Post condition:
    Inits the communication UART for non blocking TX and RX

  Params:
    None

  Return:
    INIT_OK, if Initialization succesfull
    INIT_FAILED, if initialization falied

*******************************************************************************/
UINT8 Uart_HW_Init(void)
{
  UINT8 retVal = INIT_FAILED;

  /* PTB Clock Gate Control: Clock enabled */
  CLOCK_EnableClock(kCLOCK_PortB);

  /* PORTB16 (pin 51) is configured as LPUART0_RX */
  PORT_SetPinMux(PORTB, 16U, kPORT_MuxAlt3);

  /* PORTB17 (pin 52) is configured as LPUART0_TX */
  PORT_SetPinMux(PORTB, 17U, kPORT_MuxAlt3);

  SIM->SOPT5 = ((SIM->SOPT5 &
                 /* Mask bits to zero which are setting */
                 (~(SIM_SOPT5_LPUART0TXSRC_MASK | SIM_SOPT5_LPUART0RXSRC_MASK)))

                /* LPUART0 transmit data source select: LPUART0_TX pin. */
                | SIM_SOPT5_LPUART0TXSRC(SOPT5_LPUART0TXSRC_LPUART_TX)

                /* LPUART 0 receive data source select: LPUART0_RX pin. */
                | SIM_SOPT5_LPUART0RXSRC(SOPT5_LPUART0RXSRC_LPUART_RX));

  //BOARD_BootClockRUN();
  CLOCK_SetLpuartClock(1U);

  /*
   * config.baudRate_Bps = 115200U;
   * config.parityMode = kLPUART_ParityDisabled;
   * config.stopBitCount = kLPUART_OneStopBit;
   * config.txFifoWatermark = 0;
   * config.rxFifoWatermark = 0;
   * config.enableTx = false;
   * config.enableRx = false;
   */
  LPUART_GetDefaultConfig(&config);
  config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
  config.enableTx = true;
  config.enableRx = true;

  if(LPUART_Init(LPUART0, &config, CLOCK_GetFreq(kCLOCK_PllFllSelClk)) == kStatus_Success )
  {
    retVal = INIT_OK;
  }
  else
  {
    retVal = INIT_FAILED;
  }

  /* Enable RX interrupt. */
  NVIC_SetPriority(LPUART0_IRQn, 1U);
  LPUART_EnableInterrupts(LPUART0, kLPUART_RxDataRegFullInterruptEnable);
  EnableIRQ(LPUART0_IRQn);

  return retVal;
}

/******************************************************************************
  UINT8 Get_UART_Byte_Received_Flag(void)
  Gets UART flag received flag

  Pre condition:
    UART Rx byte received

  Post condition:
    Gets the flag value

  Params:
    None

  Return:
    flag value
*******************************************************************************/
UINT8 Get_UART_Byte_Received_Flag(void)
{
  return byte_received_flag;
}

/******************************************************************************
  void Clear_UART_Byte_Received_Flag(void)
  Clear byte received flag

  Pre condition:
    None

  Post condition:
    Clears byte_received_flag

  Params:
    None

  Return:
    Noner
*******************************************************************************/
void Clear_UART_Byte_Received_Flag(void)
{
  byte_received_flag = FALSE;
}

/******************************************************************************
  UINT8 Get_Byte_RX_From_Comm_UART(void)
  returns UART byte received

  Pre condition:
    UART Rx interrupt received

  Post condition:
    returns a byte received from UART

  Params:
    None

  Return:
    UART received byte
*******************************************************************************/
UINT8 Get_Byte_RX_From_Comm_UART(void)
{
  return byte_received;
}

/******************************************************************************
UINT32 Get_Comm_UART_Hw_BaudRate(void)
  Returns the current comm UART BaudRate

  Pre condition:
    UART must be initialized

  Post condition:
    None

  Params:
    None

  Return:
    Returns the baud rate

*******************************************************************************/
UINT32 Get_Comm_UART_Hw_BaudRate(void)
{
  return config.baudRate_Bps;
}

/******************************************************************************
UINT8 Get_Frame_Sent_Flag(void)
  Returns the value of the serial flag sent

  Pre condition:
    None

  Post condition:
    None

  Params:
    None

  Return:
    Returns the flag value

*******************************************************************************/
UINT8 Get_Frame_Sending_Flag(void)
{
  return serial_frame_sending_flag;
}

/******************************************************************************
UINT8 Send_UART_Frame(UINT8 *data_To_Transfer, UINT8 data_To_Transfer_Lenght)
  Sends a frame in non blocking mode

  Pre condition:
    a Tx transfer must have been initialized

  Post condition:
    clears the uart frame sending flag

  Params:
    - UINT8 *data_To_Transfer
    - UINT8 data_To_Transfer_Lenght

  Return:
    None

*******************************************************************************/
void Send_UART_Frame(UINT8 *data_To_Transfer, UINT8 data_To_Transfer_Lenght)
{
  //copy buffer to send it, non blocking mode
  bytes_to_Send = data_To_Transfer_Lenght;
  memcpy(serial_Tx_Data_Buffer,data_To_Transfer,data_To_Transfer_Lenght);

  serial_frame_sending_flag = TRUE;
  LPUART_EnableInterrupts(LPUART0, kLPUART_TxDataRegEmptyInterruptEnable);
}

str_serial_data UART_Get_serial_data_Rx_struct(void)
{
  str_serial_data mySerialData;

  mySerialData.pSerialDataBuffer = &serial_Data_Buffer[0];
  mySerialData.serialDataBuffer_Elements = serial_Buffer_Index;

  return mySerialData;
}

void UART_Clear_Rx_Buffer(void)
{
  serial_Buffer_Index = 0;
}

/******************************************************************************
  void LPUART0_IRQHandler(void)
   ISR for communications UART

  Pre condition:
    UART must be initialized

  Post condition:
    manages the Rx and Tx data

  Params:
    None

  Return:
    None

*******************************************************************************/
void LPUART0_IRQHandler(void)
{

  /* If new data arrived. */
  if ((kLPUART_RxDataRegFullFlag)&LPUART_GetStatusFlags(LPUART0))
  {

    //check if its starting byte and its a new frame
    byte_received_flag = TRUE;

    byte_received = LPUART_ReadByte(LPUART0);

    serial_Data_Buffer[serial_Buffer_Index] = byte_received;
    serial_Buffer_Index = serial_Buffer_Index + 1;

    #ifdef SERIAL_UART_ECHO
      //check if TxReg is emptey and uart flags are cleared
      if((kLPUART_TxDataRegEmptyFlag & LPUART_GetStatusFlags(LPUART0)))
      {
        LPUART_WriteByte(LPUART0, byte_received );
      }
    #endif
  }
  else
  {
    /* byte send */
    if ((kLPUART_TxDataRegEmptyFlag)&LPUART_GetStatusFlags(LPUART0))
    {
      //check if there are bytes left to send
      if(bytes_to_Send > 0)
      {
        LPUART_WriteByte(LPUART0, serial_Tx_Data_Buffer[bytes_Sent]);
        bytes_Sent = bytes_Sent + 1;
        bytes_to_Send = bytes_to_Send - 1;
      }
      else
      {
        //all serial data sent, clear variables and disable tx interrupt
        bytes_Sent = 0;
        bytes_to_Send = 0;

        //only Rx interrupt enabled
        LPUART_DisableInterrupts(LPUART0, kLPUART_TxDataRegEmptyInterruptEnable);

        serial_frame_sending_flag = FALSE;
      }
    }
  }

  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
    exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
