/*
 * crc_tools.h
 *
 *  Created on: 8 feb. 2019
 *      Author: MAX PC
 */

#ifndef CRC_TOOLS_CRC_TOOLS_H_
#define CRC_TOOLS_CRC_TOOLS_H_


#include "globals.h"

/**
 * This function calculates the CRC from a chunck of bytes
 * @param[in] crc previous CRC value
 * @param[in] buffer data buffer to extract the crc
 * @param[in] len lenght of the data array
 */
UINT16 Crc16Calc(UINT16 crc, UINT8 *buffer, UINT16 len);

#endif /* SRC_MAINAPP_NODCONFMAN_CRC16_H_ */
