/*
 * base64_codec.c
 *
 *  Created on: 6 feb. 2019
 *      Author: MAX PC
 */

#include "Serial_Manager/base64_codec.h"
#include "globals.h"

//*****************************************************************************
//GLOBAL VARIABLES
//*****************************************************************************
const char kBase64Alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

//*****************************************************************************
UINT8 B64Encode(const char* input, UINT16 input_length, char* out,UINT16 out_length)
//*****************************************************************************
// description: Base64 frame encoder
//*****************************************************************************
{
    int i = 0, j = 0;
    char *out_begin = out;
    unsigned char a3[3];
    unsigned char a4[4];

    UINT16 encoded_length = B64EncodedLength(input_length);

    if (out_length < encoded_length)
        return false;

    while (input_length--)
    {
        a3[i++] = *input++;
        if (i == 3)
        {
            B64a3_to_a4(a4, a3);

            for (i = 0; i < 4; i++)
            {
                *out++ = kBase64Alphabet[a4[i]];
            }

            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 3; j++)
        {
            a3[j] = '\0';
        }

        B64a3_to_a4(a4, a3);

        for (j = 0; j < i + 1; j++)
        {
            *out++ = kBase64Alphabet[a4[j]];
        }

        while ((i++ < 3))
        {
            *out++ = '=';
        }
    }
    return (out == (out_begin + encoded_length));
}

//*****************************************************************************
UINT8 B64Decode(const char* input, UINT16 input_length, char* out,UINT16 out_length)
//*****************************************************************************
// description: Base64 frame Decoder
//*****************************************************************************
{
    int i = 0, j = 0;
    char *out_begin = out;
    unsigned char a3[3];
    unsigned char a4[4];

    UINT16 decoded_length = B64DecodedLength(input, input_length);

    if (out_length < decoded_length)
        return false;

    while (input_length--)
    {
        if (*input == '=')
        {
            break;
        }

        a4[i++] = *(input++);
        if (i == 4)
        {
            for (i = 0; i < 4; i++)
            {
                a4[i] = B64_lookup(a4[i]);
            }

            B64a4_to_a3(a3, a4);

            for (i = 0; i < 3; i++)
            {
                *out++ = a3[i];
            }

            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 4; j++)
        {
            a4[j] = '\0';
        }

        for (j = 0; j < 4; j++)
        {
            a4[j] = B64_lookup(a4[j]);
        }

        B64a4_to_a3(a3, a4);

        for (j = 0; j < i - 1; j++)
        {
            *out++ = a3[j];
        }
    }

    return (out == (out_begin + decoded_length));
}

//*****************************************************************************
int B64EncodedLength(UINT16 length)
//*****************************************************************************
// description: Base64 length of the encoded frame
//*****************************************************************************
{
    return (length + 2 - ((length + 2) % 3)) / 3 * 4;
}

//*****************************************************************************
int B64DecodedLength(const char* in, UINT16 in_length)
//*****************************************************************************
// description: Base64 length of the decoded frame
//*****************************************************************************
{
    int numEq = 0;

    const char *in_end = in + in_length;
    while (*--in_end == '=')
    {
        ++numEq;
    }
    return ((6 * in_length) / 8) - numEq;
}

void B64a3_to_a4(unsigned char* a4, unsigned char* a3)
{
    a4[0] = (a3[0] & 0xfc) >> 2;
    a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
    a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
    a4[3] = (a3[2] & 0x3f);
}

void B64a4_to_a3(unsigned char* a3, unsigned char* a4)
{
    a3[0] = (a4[0] << 2) + ((a4[1] & 0x30) >> 4);
    a3[1] = ((a4[1] & 0xf) << 4) + ((a4[2] & 0x3c) >> 2);
    a3[2] = ((a4[2] & 0x3) << 6) + a4[3];
}

unsigned char B64_lookup(unsigned char c)
{
    if (c >= 'A' && c <= 'Z')
        return c - 'A';
    if (c >= 'a' && c <= 'z')
        return c - 71;
    if (c >= '0' && c <= '9')
        return c + 4;
    if (c == '+')
        return 62;
    if (c == '/')
        return 63;
    return 255;
}




