//
//  Optimized Crypto Library for OberonHAP.
//  Copyright 2016 Oberon microsystems, Inc.
//

#ifndef OCC_CHACHA20_LOOP_H
#define OCC_CHACHA20_LOOP_H

#include <stdint.h>


/**
 * Chacha20 inner loop
 *
 * @param[in,out] x State.
 */
void occ_chacha20loop(uint32_t x[16]);

#endif
