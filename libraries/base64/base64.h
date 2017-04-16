/**
 *  Base64.c Algortihm for base 64 encoding, modified to be url-safe by using "-", "_" and "." instead of "+", "/" and "="
 *  source https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64
 *  License is CC-SA or public domain, please check link above for details
 */

/**
 *  Encodes a stream of binary data into base64 ascii string. 
 *  endocoding efficiency is 75%, i.e. you need 4 chars (32 bits) to represent 24 bits of data
 *  Please note the implementation uses "-", "_" and "." as characters instead of standard
 *  "+". "/", "=" for url-safety
 *
 *  @param data_buf data to encode
 *  @param dataLength length of data_buf
 *  @param result result buffer
 *  @param resultSize size of result buffer
 *  @return 1 on error, 0 on success.
 */
 
#ifndef BASE64_H
#define BASE64_H

#include <stdlib.h>

int base64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize);

#endif
