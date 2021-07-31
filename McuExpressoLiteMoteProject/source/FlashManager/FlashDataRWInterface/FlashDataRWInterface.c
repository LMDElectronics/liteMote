/*
 * FlashDataRWInterface.c
 *
 *  Created on: 3 may. 2020
 *      Author: MAX PC
 */

/*
 * This module is tuned for mx25l128 memory,
 */

#include <CRC_tools/crc_tools.h>
#include <External_HW_Drivers/mx25l128/mx25l128.h>
#include <FlashManager/FlashDataRWInterface/FlashDataRWInterface.h>

//UINT8 flashSwapBuffer[MX25L128_MAX_PAGE_SIZE * MAX_PAGES_USED_FOR_SYSTEM_DATA];

void* getSP(void)
{
    void *sp;
    __asm__ __volatile__ ("mov %0, sp" : "=r"(sp));
    return sp;
}

//*****************************************************************************
void FlashInterface_Init()
//*****************************************************************************
//
//*****************************************************************************
{
  mx25l128_Init();
}

//*****************************************************************************
void FlashInterface_Save_SystemData_InFlash(void *dataBuffer, UINT16 flashPage, UINT8 memPageDataOffset, UINT8 numBytesToWrite)
//*****************************************************************************
// Using the msgType parameter, the module knows where to save the data into
// the external flash, and how to do it
//*****************************************************************************
{
  UINT8 flashSwapBuffer[MX25L128_MAX_PAGE_SIZE * MAX_PAGES_USED_FOR_SYSTEM_DATA];

  UINT8 *dataRead;
  UINT16 index_page;
  UINT16 index_swap_buffer;
  UINT8 index_data;
  UINT8 j;
  UINT16 dataBufferCRC;

  //casting data
  UINT8 *data = dataBuffer;

  //1- read all system flash data pages and save it to "flashSwapBuffer"
  //2- erase first sector of 4Kb where all system data should be
  //3- modify needed bytes in "flashSwapBuffer"
  //4- save "flashSwapBuffer" into external flash accordingly
  //5- calculate and save crc from data saved next to data

  //read pages and save then in RAM buffer
  index_swap_buffer = 0;
  for(j=0; j<3; j++)
  {
    mx25l128_Read_Page(FLASH_PAGE_SYSTEM_DATA);
    while(!mx25l128_CheckForOperationFinished());

    dataRead = mx25l128_GetReadPage();

    for(index_page=0; index_page < MX25L128_MAX_PAGE_SIZE; index_page++)
    {
      flashSwapBuffer[index_swap_buffer++] = dataRead[index_page];
    }
  }

  //erase sector 0
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

  //calculate the data crc
  dataBufferCRC = Crc16Calc(0, dataBuffer, numBytesToWrite);

  //load the page and offset inside the RAM swap data
  index_swap_buffer = ((2 << 8) * flashPage) + memPageDataOffset;

  //save data
  index_data=0;
  for(j=index_swap_buffer; j<(index_swap_buffer + numBytesToWrite); j++)
  {
    flashSwapBuffer[j] = data[index_data++];
  }

  //save CRC
  flashSwapBuffer[j++] = (UINT8)((dataBufferCRC & 0xFF00) >> 8);
  flashSwapBuffer[j++] = (UINT8)(dataBufferCRC & 0x00FF);

  //Save the system pages in external flash with the new updated data
  for(j=0; j<3; j++)
  {
    mx25l128_Set_Write_Enable();
    while(!mx25l128_CheckForOperationFinished());

    mx25l128_Write_Page(j, &flashSwapBuffer[j * MX25L128_MAX_PAGE_SIZE], MX25L128_MAX_PAGE_SIZE);
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
  }
}

//*****************************************************************************
UINT8 FlashInterface_Load_SystemData_FromFlash(UINT8 *dataBuffer, UINT16 flashPage, UINT8 memPageDataOffset, UINT8 numBytesToLoad)
//*****************************************************************************
// Loads the system data from flash
//*****************************************************************************
{
  UINT8 *dataPageRead;
  UINT16 dataCrc;
  UINT8 i;
  UINT8 retVal = FAILED;

  mx25l128_Read_Page(flashPage);
  while(!mx25l128_CheckForOperationFinished());

  dataPageRead = mx25l128_GetReadPage();

  for(i=0; i<numBytesToLoad; i++)
  {
    dataBuffer[i] = dataPageRead[memPageDataOffset + i];
  }

  //load data CRC
  dataCrc = (UINT16)((dataPageRead[memPageDataOffset + i++]) << 8);
  dataCrc |= (UINT16)(dataPageRead[memPageDataOffset + i++]);

  //calculate data CRC
  if(Crc16Calc(0, dataBuffer, numBytesToLoad) == dataCrc)
  {
    retVal = OK;
  }
  else
  {
    retVal = FAILED;
  }

  return retVal;
}

//*****************************************************************************
void FlashInterface_Save_FWTableData_InFlash(void *dataBuffer, UINT8 fwTablePosition)
//*****************************************************************************
// Saves the FW table data into external flash
//*****************************************************************************
{

}

