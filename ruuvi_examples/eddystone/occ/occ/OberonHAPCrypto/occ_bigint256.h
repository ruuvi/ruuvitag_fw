//
//  Optimized Crypto Library for OberonHAP.
//  Copyright 2016 Oberon microsystems, Inc.
//

#ifndef OCC_BIGINT256_H
#define OCC_BIGINT256_H

#include <stdint.h>
#include "occ_cpu_dep.h"


#ifndef occ_load_littleendian

/**
 * load a word from 4 bytes in memory, unaligned little endian 
 */
uint32_t occ_load_littleendian(const uint8_t x[4]);

/**
 * store a word to 4 bytes in memory, unaligned little endian
 */
void occ_store_littleendian(uint8_t x[4], uint32_t u);

/**
 * load a word from 4 bytes in memory, unaligned big endian 
 */
uint32_t occ_load_bigendian(const uint8_t x[4]);

/**
 * store a word to 4 bytes in memory, unaligned big endian
 */
void occ_store_bigendian(uint8_t x[4], uint32_t u);

#endif


/**
 * 128 bit add
 *
 * `r = a + b`
 *
 * @param[out] r Sum of @p a and @p b.
 * @param      a First summand.
 * @param      b Second summand.
 *
 * @returns carry out (0 or 1).
 *
 * @remark @p r may be same as @p a or @p b.
 */
int occ_bigint_add128(uint32_t r[4], const uint32_t a[4], const uint32_t b[4]);

/**
 * 256 bit add
 *
 * `r = a + b`
 *
 * @param[out] r Sum of @p a and @p b.
 * @param      a First summand.
 * @param      b Second summand.
 *
 * @returns carry out (0 or 1).
 *
 * @remark @p r may be same as @p a or @p b.
 */
int occ_bigint_add256(uint32_t r[8], const uint32_t a[8], const uint32_t b[8]);

/**
 * 256 bit subtract
 *
 * `r = a - b`
 *
 * @param[out] r Difference of @p a and @p b.
 * @param      a Minuend.
 * @param      b Subtrahend.
 *
 * @returns carry out (0 or 1).
 *
 * @remark @p r may be same as @p a or @p b.
 */
int occ_bigint_sub256(uint32_t r[8], const uint32_t a[8], const uint32_t b[8]);

/**
 * 128 bit multiply, 256 bit result
 *
 * `r = a * b`
 *
 * @param[out] r Product of @p a and @p b.
 * @param      a First factor.
 * @param      b Second factor.
 */
void occ_bigint_mul128(uint32_t r[8], const uint32_t a[4], const uint32_t b[4]);

/**
 * 256 bit multiply, 512 bit result
 *
 * `r = a * b`
 *
 * @param[out] r Product of @p a and @p b.
 * @param      a First factor.
 * @param      b Second factor.
 */
void occ_bigint_mul256(uint32_t r[16], const uint32_t a[8], const uint32_t b[8]);

/**
 * 256 bit square, 512 bit result
 *
 * `r = a ^ 2`
 *
 * @param[out] r Square of @p a.
 * @param      a Base.
 */
void occ_bigint_sqr256(uint32_t r[16], const uint32_t a[8]);

/**
 * conditional 256 bit add
 *
 * `r += b ? a : 0`
 *
 * @returns carry out (0 or 1).
 *
 * @param[in,out] r @p a is added if @p b = 1.
 * @param         a Summand to add to @p r if condition @p b = 1.
 * @param         b Condition. Must be 0 or 1.
 */
int occ_bigint_cadd256(uint32_t r[8], const uint32_t a[8], int b);

/**
 * conditional 256 bit subtract
 *
 * `r -= b ? a : 0`
 *
 * @returns carry out (0 or 1).
 *
 * @param[in,out] r @p a is added if @p b = 1.
 * @param         a Summand to add to @p r if condition @p b = 1.
 * @param         b Condition. Must be 0 or 1.
 */
int occ_bigint_csub256(uint32_t r[8], const uint32_t a[8], int b);

/**
 * conditional 256 bit swap
 *
 * `(x,y) = b ? (y,x) : (x,y)`
 *
 * @param[in,out] x Is swapped with @p y if @p b = 1.
 * @param[in,out] y Is swapped with @p x if @p b = 1.
 * @param         b Condition. Must be 0 or 1.
 */
void occ_bigint_cswap256(uint32_t x[8], uint32_t y[8], int b);

#endif
