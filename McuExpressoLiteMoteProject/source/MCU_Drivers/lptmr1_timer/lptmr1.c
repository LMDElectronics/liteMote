/*
 * health_timer.c
 *
 *  Created on: 17 may. 2020
 *      Author: MAX PC
 */

#include <MCU_Drivers/lptmr1_timer/lptmr1.h>
#include "fsl_clock.h"
#include "fsl_lptmr.h"
#include "clock_config.h"

volatile UINT8 flag_Need_Tx_Health = FALSE;

//*****************************************************************************
// lptmr1 used for Health timer
//*****************************************************************************

//*****************************************************************************
void Health_Timer_Init(void)
//*****************************************************************************
// Inits the health msg periodic timer
//*****************************************************************************
{
  //use LPTMR1
  UINT32 reg=0;

  //LPTMR1 interrupt is not part of the 32 formerly interrupts routed directly in arm m0 core, its needed to route it through
  //the imux peripheral
  //init the IMUX0 channel 1 peripheral
  CLOCK_EnableClock(kCLOCK_Lptmr1);
  CLOCK_EnableClock(kCLOCK_Intmux0);
  INTMUX0->CHANNEL[1].CHn_CSR |= INTMUX_CHn_CSR_RST_MASK;
  NVIC_EnableIRQ(INTMUX0_1_IRQn);

  //set the logic of the interupts attached to the imux0 peripheral, OR logic selected
  INTMUX0->CHANNEL[1].CHn_CSR = INTMUX_CHn_CSR_AND(0);

  //wire interrupt
  INTMUX0->CHANNEL[1].CHn_IER_31_0 |= (1U << ((uint32_t)LPTMR1_IRQn - FSL_FEATURE_INTMUX_IRQ_START_INDEX));

  //Enable at the NVIC
  NVIC_SetPriority(LPTMR1_IRQn, 0);
  EnableIRQ(LPTMR1_IRQn);

  //loading max CMP value
  LPTMR1->CMR = (UINT32)(0xFFFF);

  //prescaler -> enabled -> 1024
  //clock LPO 1khz
  //1 tick every 1,024 seconds
  LPTMR1->PSR = 0x49;

  //enable interrupts
  //reg &= ~(0x80);
  reg |= 0x80;
  reg |= 0x40;
  LPTMR1->CSR = reg;

}

//*****************************************************************************
void Health_Timer_Load_Counter_CMP(UINT16 data)
//*****************************************************************************
// 1000 = LPO clock 1khz
// 1024 = presscaler
//*****************************************************************************
{
  LPTMR1->CMR = (UINT32)(data);
}

//*****************************************************************************
void Health_Timer_Start(void)
//*****************************************************************************
// health timer start
//*****************************************************************************
{
  UINT32 reg = LPTMR1->CSR;

  //reset tcf flag and start timer
  reg &= ~(0x80);
  reg |= 0x01;

  LPTMR1->CSR = reg;
}

//*****************************************************************************
void Health_Timer_Stop(void)
//*****************************************************************************
// health timer start
//*****************************************************************************
{
  LPTMR1->CSR &= 0xFE;
}

//*****************************************************************************
void Health_Timer_Test(void)
//*****************************************************************************
// test for health timer 5 segs every lptmr interrupt
//*****************************************************************************
{
  Health_Timer_Init();
  Health_Timer_Load_Counter_CMP(60); // 10segs
  Health_Timer_Start();
}

//*****************************************************************************
void Reset_Tx_Health_Flag(void)
//*****************************************************************************
// Resets the Tx health flag
//*****************************************************************************
{
  flag_Need_Tx_Health = FALSE;
}

//*****************************************************************************
UINT8 Get_Tx_Health_Flag(void)
//*****************************************************************************
// Gets the status flag of health msg need to be sent
//*****************************************************************************
{
  return flag_Need_Tx_Health;
}

//*****************************************************************************
void INTMUX0_1_IRQHandler(void)
//*****************************************************************************
// IntMUX0 must call SP1 isr
//*****************************************************************************
{
  uint32_t pendingIrqOffset;

  pendingIrqOffset = INTMUX0->CHANNEL[1].CHn_VEC;
  if (pendingIrqOffset)
  {
      uint32_t isr = *(uint32_t *)(SCB->VTOR + pendingIrqOffset);
      ((void (*)(void))isr)();
  }
}

//*****************************************************************************
void LPTMR1_IRQHandler(void)
//*****************************************************************************
// LPTMR1 ISR
//*****************************************************************************
{
  LPTMR1->CSR |= 0x80;

  flag_Need_Tx_Health = TRUE;
  /*
   * Workaround for TWR-KV58: because write buffer is enabled, adding
   * memory barrier instructions to make sure clearing interrupt flag completed
   * before go out ISR
   */
  __DSB();
  __ISB();
}
