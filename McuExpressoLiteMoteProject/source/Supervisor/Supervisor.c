/*
 * error_report_manager.c
 *
 *  Created on: 11 abr. 2020
 *      Author: MAX PC
 */

#include <Supervisor/Supervisor.h>

//*****************************************************************************
void Init_Pinout_Error_Supervisor(void)
//*****************************************************************************
// Inits the module pinout
//*****************************************************************************
{
  gpio_pin_config_t led_config = {kGPIO_DigitalOutput, 1};

  /* PTC Clock Gate Control: Clock enabled */
  CLOCK_EnableClock(kCLOCK_PortC);

  //pin 56 Red from RGB led
  PORT_SetPinMux(PORTC, 1U, kPORT_MuxAsGpio);
  GPIO_PinInit(GPIOC, 1, &led_config);

}

//*****************************************************************************
void Init_Supervisor(void)
//*****************************************************************************
// Inits the error report module
//*****************************************************************************
{
  Init_Pinout_Error_Supervisor();
}

void Supervisor_SetLed_RED(void)
{
  GPIO_PortClear(GPIOC, 1u << 1);
}

void Supervisor_ClearLed_RED(void)
{
  GPIO_PortSet(GPIOC, 1u << 1);
}

//*****************************************************************************
void SignalStopError(UINT8 stopError_Code)
//*****************************************************************************
// Signals the module error
//*****************************************************************************
{
  switch(stopError_Code)
  {
    case FLASH_STOP_ERROR:

      //signal RGB LED and stopping here HW fault encountered
      Supervisor_SetLed_RED();
      while(1);


    break;
  }
}

