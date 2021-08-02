/*
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    MKL82Z128xxx7_Project.c
 * @brief   Application entry point.
 */
#include <MCU_Drivers/adc/adc.h>
#include <MCU_Drivers/lptmr1_timer/lptmr1.h>
//#include <External_HW_Drivers/mx25l128/mx25l128.h>
//#include <External_HW_Drivers/s2lp/s2lp.h>
#include <FlashManager/FlashDataRWInterface/FlashDataRWInterface.h>
#include <RadioManager/RadioManager.h>
#include <MCU_Drivers/spi/spi.h>
#include <MCU_Drivers/uart/lpuart.h>
#include <Serial_Manager/Serial_Manager.h>
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL82Z7.h"

#include "globals.h"
#include "packet_manager/packet_manager.h"
#include "Configuration_Manager/configuration_manager.h"


#include "Health_Manager/health_manager.h"

#include "Supervisor/Supervisor.h"


int main(void)
{
  UINT8 current_state = 0;

  //init HW peripherals
  //ADC_AutoCalibration();
  BOARD_BootClockRUN();
  SPI_System_Init();
  Health_Timer_Init();

  //init FW modules
  FlashInterface_Init();
  CnfManager_Init();
  if(CnfManager_Load_Mote_Default_Config() == FAILED)
  {
    //forced stop!
    while(1);
  }

  Init_Supervisor();

  Serial_Manager_Init();
  Packet_Manager_Init();
  Health_Manager_Init();
  Radio_Manager_Init();

  //TODO: just for test
  //Send_byte_per_second();
  //mx25l128_test();
  //ADC_Test();
  //Health_Timer_Test();
  //S2lp_Test();
  //s2lp_Test_Tx_RC();
  //s2lp_Test_Rx_RC();

  while(1)
  {
    //data input motors
    Serial_Manager_RX_Motor(); //Get serial frames sent to this mote
    //Radio_Manager_Rx_Motor(); //Get radio frames sent to this mote

    //data process motor
    Packet_Manager_Process_Motor(); // process received packets [serial, radio, etc...]

    //data output motors
    Serial_Manager_Tx_Motor(); // Send serial packets if any
    //Radio_Manager_Tx_Task(); //Send radio packets if any

    //periodically report health data to host
    Health_Manager_Motor();

    //current_state = s2lp_Get_Operating_State();

  }
  return 0;
}
