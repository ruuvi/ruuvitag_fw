//
//  Optimized Crypto Library for OberonHAP.
//  Copyright 2016 Oberon microsystems, Inc.
//

#ifndef OCC_SC25519_H
#define OCC_SC25519_H

#include <stdint.h>


/**
 * ed25519 scalar modulo group order
 */
typedef struct 
{
    uint32_t w[8]; /**< little endian */
}
occ_sc25519;


/**
 * 32 byte value to scalar
 *
 * @param[out] r Conversion result.
 * @param      x Bytes to convert to scalar.
 */
void occ_sc25519_from32bytes(occ_sc25519 *r, const uint8_t x[32]);

/**
 * 64 byte value to scalar
 *
 * @param[out] r Conversion result.
 * @param      x Bytes to convert to scalar.
 */
void occ_sc25519_from64bytes(occ_sc25519 *r, const uint8_t x[64]);

/**
 * scalar to 32 byte value
 *
 * @param[out] r Conversion result.
 * @param      x Scalar to convert to bytes.
 */
void occ_sc25519_to32bytes(uint8_t r[32], const occ_sc25519 *x);

/**
 * add scalars
 *
 * `r = x + y mod m` (the ed25519 group order)
 *
 * @param[out] r Sum of @p x and @p y modulo the ed25519 group order.
 * @param      x First summand.
 * @param      y Second summand.
 */
void occ_sc25519_add(occ_sc25519 *r, const occ_sc25519 *x, const occ_sc25519 *y);

/**
 * multiply scalars
 *
 * `r = x * y mod m` (the ed25519 group order)
 *
 * @param[out] r Product of @p x and @p y modulo the ed25519 group order.
 * @param      x First factor.
 * @param      y Second factor.
 */
void occ_sc25519_mul(occ_sc25519 *r, const occ_sc25519 *x, const occ_sc25519 *y);

/**
 * setup fixed 4 bit windows
 */
void occ_sc25519_window4(int8_t r[64], const occ_sc25519 *s);

/**
 * setup sliding 4 bit windows
 */
void occ_sc25519_sliding_window4(int8_t b[253], const occ_sc25519 *s);

#endif
