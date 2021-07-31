/*
 * mx25l128.h
 *
 *  Created on: 30 mar. 2019
 *      Author: MAX PC
 */

#ifndef EXTERNALFLASHMANAGER_MX25L128_MX25L128_H_
#define EXTERNALFLASHMANAGER_MX25L128_MX25L128_H_

#include "globals.h"

#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_clock.h"
#include "fsl_common.h"

#define DATA_BUFFER_LENGTH 260 //for page program & read 4 command bytes + 256 bytes in a page

typedef struct ExternalFlashId
{
  UINT8 ManufacturerID;
  UINT8 MemoryType;
  UINT8 MemoryDensity;
}sExternalFlashId;

#define MX25L128_MAX_PAGE_SIZE                  256

#define MX25L128_MEM_DENSITY_READ_STATUS        0x18
#define MX25L128_MEM_TYPE_READ_STATUS           0x20
#define MX25L128_MEM_MANUFACTURER_READ_STATUS   0xC2

//commands to send
#define READ_IDENTIFICATION_COMMAND 0x9F
#define READ_STATUS_REGISTER        0x05
#define READ_SECURITY_REGISTER      0x2b
#define WRITE_ENABLE                0x06
#define WRITE_DISABLE               0x04
#define PAGE_PROGRAM                0x02
#define PAGE_READ                   0x03
#define CHIP_ERASE                  0x60
#define SECTOR_ERASE                0x20
#define BLOCK_ERASE                 0xD8
#define NONE                        0x00

#define MAX_SECTORS                 4096
#define MAX_BLOCKS                   256

//bytes to receive from command sent (command + data to receive)
#define READ_IDENTIFICATION_COMMAND_BYTES_TO_RX 4
#define READ_STATUS_COMMAND_BYTES_TO_RX         2
#define ERASE_SECTOR_BYTES_TO_RX                4

#define FLAG_BUSY_STATUS_MASK 0x01
#define FLASH_NOT_BUSY_BIT   0

void mx25l128_Init(void);

void mx25l128_Read_ID(void);
sExternalFlashId GetFlashID(void);

void mx25l128_Read_Status(void);
UINT8 GetFlashStatus(void);

void mx25l128_Read_Security(void);
UINT8 GetFlash_Security_Register(void);

void mx25l128_Set_Write_Enable();
void mx25l128_Disable_Write_Enable();

void mx25l128_Write_Page(UINT16 page, UINT8 *dataBuffer, UINT16 dataLength);
void mx25l128_Read_Page(UINT16 page);
UINT8 *mx25l128_GetReadPage(void);

void mx25l128_Erase_Chip(void);
void mx25l128_Erase_Sector(UINT16 sectorToErase);
void mx25l128_Erase_64K_Block(UINT8 blockToErase);

UINT8 mx25l128_CheckForOperationFinished(void);

void mx25l128_test(void);

//spi callback function
void mx25l128_SPI_Callback(volatile UINT8 *dataReceived);

#endif /* EXTERNALFLASHMANAGER_MX25L128_MX25L128_H_ */
