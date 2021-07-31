/*
 * serial_timers_hw_conf.c
 *
 *  Created on: 12 feb. 2019
 *      Author: MAX PC
 */

#include <MCU_Drivers/pit_timer/pit.h>

/******************************************************************************
  TIMER HW DEPENDANT FUNCTIONS
*******************************************************************************/
UINT8 serial_comm_timeout_flag = FALSE;

/******************************************************************************
UINT8 Serial_Timeout_Rx_Timer_Init(UINT32 baudRate)
   Inits the serial timeout timer

  Pre condition:
    None

  Post condition:
    The timeout timer gets configured

  Params:
    baudrate, to calculate the timeout in function

  Return:
    INIT_OK, if Initialization succesfull
    INIT_FAILED, if initialization falied

*******************************************************************************/
UINT8 Serial_Timeout_Rx_Timer_Init(UINT32 baudRate)
{
  /* Structure of initialize PIT */
  pit_config_t pitConfig;

  /*
   * pitConfig.enableRunInDebug = false;
   */
  PIT_GetDefaultConfig(&pitConfig);

  /* Init pit module */
  PIT_Init(PIT, &pitConfig);

  /* Set timer period for channel 0, according to at least 3 times comm char time*/
  PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, USEC_TO_COUNT((8*3*1000000/(baudRate)), CLOCK_GetFreq(kCLOCK_BusClk)));

  /* Enable timer interrupts for channel 0 */
  PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);

  /* Enable at the NVIC */
  EnableIRQ(PIT0_IRQn);

  /* Start channel 0 */
  //PIT_StartTimer(PIT, kPIT_Chnl_0);

  return INIT_OK;
}

/******************************************************************************
UINT8 Get_Timeout_Flag(void)
  Returns serial_comm_timeout_flag value

  Pre condition:
    None

  Post condition:
    None

  Params:
    None

  Return:
    serial_comm_timeout_flag valuel return

*******************************************************************************/
UINT8 Get_Timeout_Flag(void)
{
  return serial_comm_timeout_flag;
}

/******************************************************************************
UINT8 Clear_Timeout_Flag(void)
  Clears serial_comm_timeout_flag value

  Pre condition:
    None

  Post condition:
    None

  Params:
    serial_comm_timeout_flag value

  Return:
    None

*******************************************************************************/
void Clear_Timeout_Flag(void)
{
  serial_comm_timeout_flag = FALSE;
}

/******************************************************************************
void Start_Comm_Timeout_Timer(void)
  Start Comm Timeout Timer

  Pre condition:
    None

  Post condition:
    Stops ths timer

  Params:
    None

  Return:
    None

*******************************************************************************/
void Start_Comm_Timeout_Timer(void)
{
  PIT_StartTimer(PIT, kPIT_Chnl_0);
}

/******************************************************************************
void Stop_Comm_Timeout_Timer(void)
  Stop Comm Timeout Timer

  Pre condition:
    None

  Post condition:
    Stop the timer

  Params:
    None

  Return:
    None

*******************************************************************************/
void Stop_Comm_Timeout_Timer(void)
{
  PIT_StopTimer(PIT, kPIT_Chnl_0);
}

/******************************************************************************
void PIT0_IRQHandler(void)
   ISR for serial timer rx timeout

  Pre condition:
    None

  Post condition:
    code jumps here when PIT channel 0 has overflown

  Params:
    None

  Return:
    None

*******************************************************************************/
void PIT0_IRQHandler(void)
{
    /* Clear interrupt flag.*/
    PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);

    serial_comm_timeout_flag = TRUE;

    __DSB();
}
