/*
 * base64_codec.h
 *
 *  Created on: 6 feb. 2019
 *      Author: MAX PC
 */

#ifndef SERIAL_MANAGER_BASE64_CODEC_H_
#define SERIAL_MANAGER_BASE64_CODEC_H_

#include "globals.h"

#define BASE64_FRAME_START_BYTE     0x3C // ASCII '<'
#define BASE64_FRAME_STOP_BYTE      0x3E // ASCII '>'

/*
  MAX_PACKET_SIZE * 1,33 (serial frames are 33% longer encoded in base64)
  180 bytes is big enough to hold 128 decoded serial frames
*/
#define MAX_SERIAL_FRAME_ENCODED_SIZE 180

#define false 0
#define true  1

UINT8 B64Encode(const char *input, UINT16 input_length, char *out, UINT16 out_length);
UINT8 B64Decode(const char *input, UINT16 input_length, char *out, UINT16 out_length);
int B64DecodedLength(const char *in, UINT16 in_length);
void B64a3_to_a4(unsigned char * a4, unsigned char * a3);
void B64a4_to_a3(unsigned char * a3, unsigned char * a4);
unsigned char B64_lookup(unsigned char c);

int B64EncodedLength(UINT16 length);

#endif /* 01___SERIAL_MANAGER_BASE64_CODEC_H_ */
