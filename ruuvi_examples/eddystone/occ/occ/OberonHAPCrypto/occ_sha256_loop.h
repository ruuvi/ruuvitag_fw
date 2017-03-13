//
//  Optimized Crypto Library for OberonHAP.
//  Copyright 2016 Oberon microsystems, Inc.
//

#ifndef OCC_SHA256_LOOP_H
#define OCC_SHA256_LOOP_H

#include <stdint.h>


/**
 * First SHA-256 inner loop.
 */
void occ_sha256_loop1(const uint32_t *cptr, uint32_t w[16], uint32_t v[8]);

/**
 * Second SHA-256 inner loop.
 */
void occ_sha256_loop2(uint32_t w[16]);


#endif
