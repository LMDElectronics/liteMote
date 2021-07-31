/*
 * globals.h
 *
 *  Created on: 1 feb. 2019
 *      Author: MAX PC
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "typedef.h"

#define INIT_FAILED 1
#define INIT_OK     0

#define BUSY 1
#define NOT_BUSY 0

#define OK      0
#define FAILED  1

#define TRUE    1
#define FALSE   0

#define ENTER_ATOMIC() asm("cpsid i" : : : "memory");
#define LEAVE_ATOMIC() asm("cpsie i" : : : "memory");

//debug defines
//#define SERIAL_UART_ECHO

#endif /* GLOBALS_H_ */
