//
//  Optimized Crypto Library for OberonHAP.
//  Copyright 2016 Oberon microsystems, Inc.
//

#ifndef OCC_SHA512_LOOP_H
#define OCC_SHA512_LOOP_H

#include <stdint.h>


/**
 * First SHA-512 inner loop.
 */
void occ_sha512_loop1(const uint64_t *cptr, uint64_t w[16], uint64_t v[8]);

/**
 * Second SHA-512 inner loop.
 */
void occ_sha512_loop2(uint64_t w[16]);


#endif
