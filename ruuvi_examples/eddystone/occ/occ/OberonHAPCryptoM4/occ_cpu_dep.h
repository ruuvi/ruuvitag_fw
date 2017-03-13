//
//  Optimized Crypto Library for ARM Cortex-M4.
//  Copyright 2016 Oberon microsystems, Inc.
//

#ifndef OCC_CPU_DEP_H
#define OCC_CPU_DEP_H

#include <stdint.h>


#define _noinline_ __attribute__((noinline))

#if defined(__GNUC__)
    #define __rev __builtin_bswap32
#endif

// unaligned 32 bit word
typedef struct __attribute__((packed)) {
    uint32_t w;
} occ_uauint32;

// load a word from 4 bytes in memory, unaligned little endian 
#define occ_load_littleendian(x) (((occ_uauint32*)(x))->w)

// store a word to 4 bytes in memory, unaligned little endian
#define occ_store_littleendian(x, u) ((occ_uauint32*)(x))->w = u

// load a word from 4 bytes in memory, unaligned big endian 
#define occ_load_bigendian(x) (__rev(((occ_uauint32*)(x))->w))

// store a word to 4 bytes in memory, unaligned big endian
#define occ_store_bigendian(x, u) ((occ_uauint32*)(x))->w = __rev(u)

#endif
