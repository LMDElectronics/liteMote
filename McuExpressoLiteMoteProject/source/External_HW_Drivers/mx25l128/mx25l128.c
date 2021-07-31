/*
 * mx25l128.c
 *
 *  Created on: 30 mar. 2019
 *      Author: MAX PC
 */

#include <External_HW_Drivers/mx25l128/mx25l128.h>
#include <MCU_Drivers/spi/spi.h>

sExternalFlashId mx25l128_id;
UINT8 dataToTxRx[DATA_BUFFER_LENGTH];

UINT8 status_register = 0;
UINT8 security_register = 0;

UINT8 flag_flash_operation_finished = FALSE;

UINT8 mx25l128_spi_header = 0;

//*****************************************************************************
void mx25l128_SPI_Callback(volatile UINT8 *dataReceived)
//*****************************************************************************
// spi data received callback
//*****************************************************************************
{
  switch(mx25l128_spi_header)
  {
    case READ_STATUS_REGISTER:
      status_register = dataToTxRx[1];
    break;

    case READ_SECURITY_REGISTER:
      security_register = dataToTxRx[1];
    break;
  }

  flag_flash_operation_finished = TRUE;
  Disable_CS(CS_EXTERNAL_FLASH);
}

//*****************************************************************************
void mx25l128_pinout_Init(void)
//*****************************************************************************
// mxl128 external flash pinout config
//*****************************************************************************
{
  gpio_pin_config_t led_config = {kGPIO_DigitalOutput, 1};
  gpio_pin_config_t wp_config = {kGPIO_DigitalOutput, 1};

  //external flash pinout
  /* PORTE3 (pin 4) is configured as GPIO for WP! */
  PORT_SetPinMux(PORTE, 3U, kPORT_MuxAsGpio);
  GPIO_PinInit(GPIOE, 3, &wp_config);

  //set wp = 1
  GPIO_PortSet(GPIOE, 1u << 3);

  /* PORTE5 (pin 8) is configured as SPI_EXTERNAL_FLASH_CS */
  PORT_SetPinMux(PORTE, 5U, kPORT_MuxAsGpio);
  GPIO_PinInit(GPIOE, 5, &led_config);

}

//*****************************************************************************
void mx25l128_Init(void)
//*****************************************************************************
//
//*****************************************************************************
{
  mx25l128_pinout_Init();
}

//*****************************************************************************
sExternalFlashId GetFlashID(void)
//*****************************************************************************
// returns flash id
//*****************************************************************************
{
  return mx25l128_id;
}

//*****************************************************************************
UINT8 GetFlashStatus(void)
//*****************************************************************************
// returns the flash status bytes
//*****************************************************************************
{
  return status_register;
}

//*****************************************************************************
UINT8 GetFlash_Security_Register(void)
//*****************************************************************************
// returns security register
//*****************************************************************************
{
  return security_register;
}

//*****************************************************************************
UINT8 mx25l128_CheckForOperationFinished(void)
//*****************************************************************************
// returns the operation flag
//*****************************************************************************
{
  return flag_flash_operation_finished;
}

//*****************************************************************************
void mx25l128_Read_ID(void)
//*****************************************************************************
// Read the mx25l128 id
//*****************************************************************************
{
  flag_flash_operation_finished = FALSE;

  dataToTxRx[0] = READ_IDENTIFICATION_COMMAND;

  mx25l128_spi_header = READ_IDENTIFICATION_COMMAND;

  Enable_CS(CS_EXTERNAL_FLASH);
  SPI_Send_NonBlocking(SPI1, dataToTxRx, READ_IDENTIFICATION_COMMAND_BYTES_TO_RX);
}

//*****************************************************************************
void mx25l128_Read_Status(void)
//*****************************************************************************
// read status byte from mx25l128
//*****************************************************************************
{
  flag_flash_operation_finished = FALSE;

  dataToTxRx[0] = READ_STATUS_REGISTER;
  dataToTxRx[1] = 0;

  mx25l128_spi_header = READ_STATUS_REGISTER;

  Enable_CS(CS_EXTERNAL_FLASH);
  SPI_Send_NonBlocking(SPI1, dataToTxRx, READ_STATUS_COMMAND_BYTES_TO_RX);
}

//*****************************************************************************
void mx25l128_Read_Security(void)
//*****************************************************************************
// read status byte from mx25l128
//*****************************************************************************
{
  flag_flash_operation_finished = FALSE;

  dataToTxRx[0] = READ_SECURITY_REGISTER;

  mx25l128_spi_header = READ_SECURITY_REGISTER;

  Enable_CS(CS_EXTERNAL_FLASH);
  SPI_Send_NonBlocking(SPI1, dataToTxRx, READ_STATUS_COMMAND_BYTES_TO_RX);
}

//*****************************************************************************
void mx25l128_Write_Page(UINT16 page, UINT8 *dataBuffer, UINT16 dataLength)
//*****************************************************************************
// Writes dataBuffer into page selected
//
// Refer to page 23 of 25l128 macronix datasheet to check more details about
// the operation
//*****************************************************************************
{
  UINT16 i=0;

  flag_flash_operation_finished = FALSE;

  dataToTxRx[0] = PAGE_PROGRAM;
  dataToTxRx[1] = (UINT8)((page & 0xFF00) >> 8);
  dataToTxRx[2] = (UINT8)(page & 0x00FF);
  dataToTxRx[3] = 0;

  for(i=0; i<dataLength; i++)
  {
    dataToTxRx[4 + i] = dataBuffer[i];
  }

  mx25l128_spi_header = PAGE_PROGRAM;

  Enable_CS(CS_EXTERNAL_FLASH);
  SPI_Send_NonBlocking(SPI1, dataToTxRx, dataLength + 4); //+4 bytes command & address
}

//*****************************************************************************
void mx25l128_Set_Write_Enable(void)
//*****************************************************************************
// sets the staus byte write enable to enable writing in flash memory
//*****************************************************************************
{
  flag_flash_operation_finished = FALSE;

  dataToTxRx[0] = WRITE_ENABLE;

  mx25l128_spi_header = PAGE_PROGRAM;

  Enable_CS(CS_EXTERNAL_FLASH);
  SPI_Send_NonBlocking(SPI1, dataToTxRx, 1);
}

//*****************************************************************************
void mx25l128_Disable_Write_Enable(void)
//*****************************************************************************
// sets the staus byte write enable to enable writing in flash memory
//*****************************************************************************
{
  flag_flash_operation_finished = FALSE;

  dataToTxRx[0] = WRITE_DISABLE;

  mx25l128_spi_header = WRITE_DISABLE;

  Enable_CS(CS_EXTERNAL_FLASH);
  SPI_Send_NonBlocking(SPI1, dataToTxRx, 1);
}

//*****************************************************************************
UINT8 *mx25l128_GetReadPage(void)
//*****************************************************************************
// gets the page read, use after read command and after flag_operation_finished is true
//*****************************************************************************
{
  return &dataToTxRx[4];
}

//*****************************************************************************
void mx25l128_Read_Page(UINT16 page)
//*****************************************************************************
// reads page from flash (reads 256 bytes, an entire page )
//*****************************************************************************
{
  flag_flash_operation_finished = FALSE;

  dataToTxRx[0] = PAGE_READ;
  dataToTxRx[1] = (UINT8)((page & 0xFF00) >> 8);
  dataToTxRx[2] = (UINT8)(page & 0x00FF);
  dataToTxRx[3] = 0;

  mx25l128_spi_header = PAGE_READ;

  Enable_CS(CS_EXTERNAL_FLASH);
  SPI_Send_NonBlocking(SPI1, dataToTxRx, DATA_BUFFER_LENGTH + 4); //+4 bytes command & address
}

//*****************************************************************************
void mx25l128_Erase_Chip(void)
//*****************************************************************************
// reads page from flash (reads 256 bytes, an entire page )
//*****************************************************************************
{
  flag_flash_operation_finished = FALSE;

  dataToTxRx[0] = CHIP_ERASE;

  mx25l128_spi_header = CHIP_ERASE;

  Enable_CS(CS_EXTERNAL_FLASH);
  SPI_Send_NonBlocking(SPI1, dataToTxRx, 1);
}

//*****************************************************************************
void mx25l128_Erase_Sector(UINT16 sectorToErase)
//*****************************************************************************
// erase a sector (sector size 4k)
//*****************************************************************************
{
  UINT32 initAddress = 0;

  //converting data to sector address
  if(sectorToErase < MAX_SECTORS)
  {
    initAddress = (UINT32)(sectorToErase << 12);

    flag_flash_operation_finished = FALSE;

    dataToTxRx[0] = SECTOR_ERASE;
    dataToTxRx[1] = (UINT8)((initAddress & 0x00FF0000) >> 16);
    dataToTxRx[2] = (UINT8)((initAddress & 0x0000FF00) >> 8);
    dataToTxRx[3] = (UINT8)((initAddress & 0x000000FF));

    mx25l128_spi_header = SECTOR_ERASE;

    Enable_CS(CS_EXTERNAL_FLASH);
    SPI_Send_NonBlocking(SPI1, dataToTxRx, ERASE_SECTOR_BYTES_TO_RX);
  }
}

//*****************************************************************************
void mx25l128_Erase_64K_Block(UINT8 blockToErase)
//*****************************************************************************
// erase a sector (sector size 64k)
//*****************************************************************************
{
  UINT32 initAddress = 0;

  //converting data to sector address
  if(blockToErase < MAX_BLOCKS)
  {
    initAddress = (UINT32)(blockToErase << 16);

    flag_flash_operation_finished = FALSE;

    dataToTxRx[0] = BLOCK_ERASE;
    dataToTxRx[1] = (UINT8)((initAddress & 0x00FF0000) >> 16);
    dataToTxRx[2] = (UINT8)((initAddress & 0x0000FF00) >> 8);
    dataToTxRx[3] = (UINT8)((initAddress & 0x000000FF));

    mx25l128_spi_header = BLOCK_ERASE;

    Enable_CS(CS_EXTERNAL_FLASH);
    SPI_Send_NonBlocking(SPI1, dataToTxRx, ERASE_SECTOR_BYTES_TO_RX);
  }
}


//*****************************************************************************
void mx25l128_test(void)
//*****************************************************************************
// mx25l128 test driver operations
//*****************************************************************************
{
  UINT8 dataToProgram[DATA_BUFFER_LENGTH];
  UINT16 i=0;

  for(i=0; i<DATA_BUFFER_LENGTH; i++)
  {
    dataToProgram[i]=i;
  }

  //read id
  mx25l128_Read_ID();
  while(!mx25l128_CheckForOperationFinished());

  mx25l128_id.ManufacturerID = dataToTxRx[1];
  mx25l128_id.MemoryType  = dataToTxRx[2];
  mx25l128_id.MemoryDensity = dataToTxRx[3];

  mx25l128_Set_Write_Enable();
  while(!mx25l128_CheckForOperationFinished());

  mx25l128_Erase_Sector(0);
  while(!mx25l128_CheckForOperationFinished());

  //wait for flash busy
  while(1)
  {
    mx25l128_Read_Status();
    while(!mx25l128_CheckForOperationFinished());

    if((GetFlashStatus() & FLAG_BUSY_STATUS_MASK) == FLASH_NOT_BUSY_BIT)
    {
      break;
    }
  }

  //page read, wait for data transfered, and wait for flash to finish write the data
  mx25l128_Read_Page(0);
  while(!mx25l128_CheckForOperationFinished());

  //page program, wait for data transfered, and wait for flash to finish write the data

  mx25l128_Set_Write_Enable();
  while(!mx25l128_CheckForOperationFinished());

  mx25l128_Write_Page(0, dataToProgram, DATA_BUFFER_LENGTH);
  while(!mx25l128_CheckForOperationFinished());

  //wait for flash busy
  while(1)
  {
    mx25l128_Read_Status();
    while(!mx25l128_CheckForOperationFinished());

    if((GetFlashStatus() & FLAG_BUSY_STATUS_MASK) == FLASH_NOT_BUSY_BIT)
    {
      break;
    }
  }

  mx25l128_Read_Security();
  while(!mx25l128_CheckForOperationFinished());

  //page read, wait for data transfered, and wait for flash to finish write the data
  mx25l128_Read_Page(0);
  while(!mx25l128_CheckForOperationFinished());
}

