/*
 * FlashDataRWInterface.h
 *
 *  Created on: 3 may. 2020
 *      Author: MAX PC
 */

#ifndef FLASHMANAGER_FLASHDATARWINTERFACE_FLASHDATARWINTERFACE_H_
#define FLASHMANAGER_FLASHDATARWINTERFACE_FLASHDATARWINTERFACE_H_

#include "globals.h"

//define for pages used for system data R/W
#define MAX_PAGES_USED_FOR_SYSTEM_DATA 3

#define FLASH_PAGE_SYSTEM_DATA  0
#define FLASH_PAGE_ROUTE_TABLES 1
#define FLASH_PAGE_FW_TABLE     2

void FlashInterface_Init();
void FlashInterface_Save_SystemData_InFlash(void *dataBuffer, UINT16 flashPage, UINT8 memPageDataOffset, UINT8 numBytesToWrite);
UINT8 FlashInterface_Load_SystemData_FromFlash(UINT8 *dataBuffer, UINT16 flashPage, UINT8 memPageDataOffset, UINT8 numBytesToLoad);

void FlashInterface_Save_FWTableData_InFlash(void *dataBuffer, UINT8 fwTablePosition);

#endif /* 01___FLASHMANAGER_FLASHDATARWINTERFACE_FLASHDATARWINTERFACE_H_ */
