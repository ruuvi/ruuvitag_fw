//
//  Optimized Crypto Library for OberonHAP.
//  Copyright 2016 Oberon microsystems, Inc.
//

#ifndef OCC_POLY1305_MUL_H
#define OCC_POLY1305_MUL_H

#include <stdint.h>


/**
 * 130 bit modulo add multiply
 *
 * `h = (h + a) * r modulo 2^130-5`
 *
 * @param[in,out] h Result of `(h + a) * r modulo 2^130-5`, reduced to 0 <= @p h <= 2^130.
 * @param         r Factor to multiply with `(h + a)`.
 * @param         a Summand to add to @p h first.
 */
void occ_addmulmod1305(uint32_t h[5], const uint32_t r[4], const uint32_t a[5]);

/**
 * final reduction modulo 2^130-5
 *
 * @param[in,out] h Number to reduce.
 */
void occ_freeze1305(uint32_t h[5]);

#endif
