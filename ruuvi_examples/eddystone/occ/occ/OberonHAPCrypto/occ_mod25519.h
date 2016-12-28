//
//  Optimized Crypto Library for OberonHAP.
//  Copyright 2016 Oberon microsystems, Inc.
//
 
#ifndef OCC_MOD25519_H
#define OCC_MOD25519_H

#include <stdint.h>


/**
 * modulo 2^255-19 number
 */
typedef struct 
{
  uint32_t w[8]; /**< little endian */
}
occ_mod25519;


/**
 * final reduction modulo 2^255-19
 * @param[in,out] r Number to reduce.
 */
void occ_mod25519_freeze(occ_mod25519 *r);

/**
 * convert bytes to modulo number
 * @param[out] r Conversion result.
 * @param[in]  x Bytes to convert to number.
 */
void occ_mod25519_fromBytes(occ_mod25519 *r, const uint8_t x[32]);

/**
 * convert modulo number to bytes
 * @param[out] r Conversion result.
 * @param[in]  x Number to convert to bytes.
 */
void occ_mod25519_toBytes(uint8_t r[32], occ_mod25519 *x);

/**
 * `r = val`
 * @param[out] r   Result.
 * @param      val Value to assign.
 */
void occ_mod25519_init(occ_mod25519 *r, uint32_t val);

/**
 * negate modulo 2^255-19
 *
 * `r = -x mod 2^255-19`
 *
 * @param r Result.
 * @param x Value to negate.
 *
 * @remark @p r may be same as @p x.
 */
void occ_mod25519_neg(occ_mod25519 *r, const occ_mod25519 *x);

/**
 * get sign = lsb of fully reduced number
 *
 * @param[in,out] x Number to get sign of.
 *
 * @returns sign bit of @p x (0 or 1)
 *
 * @remark @p x will be fully reduced.
 */
int occ_mod25519_getsign(occ_mod25519 *x);

/**
 * compare two modulo numbers
 *
 * @param[in,out] x Number to compare with @p y.
 * @param[in,out] y Number to compare with @p x.
 *
 * @returns 1 If the two numbers are equal modulo p.
 * @returns 0 Otherwise.
 *
 * @remark @p x and @p y will be fully reduced.
 */
int occ_mod25519_equal(occ_mod25519 *x, occ_mod25519 *y);

/**
 * modulo add
 *
 * `r = (x + y) mod 2^255-19` @n
 * reduced to: 0 <= @p r < 2^256
 *
 * @param[out] r Sum of @p x and @p y modulo 2^255-19, reduced to 0 <= @p r < 2^256.
 * @param      x First summand.
 * @param      y Second summand.
 *
 * @remark @p r may be same as @p x or @p y.
 */
void occ_mod25519_add(occ_mod25519 *r, const occ_mod25519 *x, const occ_mod25519 *y);

/**
 * modulo subtract
 *
 * `r = (x - y) mod 2^255-19` @n
 * reduced to: 0 <= @p r < 2^256
 *
 * @param[out] r Difference of @p x and @p y modulo 2^255-19, reduced to 0 <= @p r < 2^256.
 * @param      x Minuend.
 * @param      y Subtrahend.
 *
 * @remark @p r may be same as @p x or @p y.
 */
void occ_mod25519_sub(occ_mod25519 *r, const occ_mod25519 *x, const occ_mod25519 *y);

/**
 * modulo multiply
 *
 * `r = (x * y) mod 2^255-19` @n
 * reduced to: 0 <= @p r < 2^256
 *
 * @param[out] r Product of @p x and @p y modulo 2^255-19, reduced to 0 <= @p r < 2^256.
 * @param      x First factor.
 * @param      y Second factor.
 *
 * @remark @p r may be same as @p x or @p y.
 */
void occ_mod25519_mul(occ_mod25519 *r, const occ_mod25519 *x, const occ_mod25519 *y);

/**
 * modulo square
 *
 * `r = x^2 mod 2^255-19` @n
 * reduced to: 0 <= @p r < 2^256
 *
 * @param[out] r Square of @p x modulo 2^255-19, reduced to 0 <= @p r < 2^256.
 * @param      x Base.
 *
 * @remark @p r may be same as @p x.
 */
void occ_mod25519_square(occ_mod25519 *r, const occ_mod25519 *x);

/**
 * modulo multiply by 121666
 *
 * `r = (x * 121666) mod 2^255-19` @n
 * reduced to: 0 <= @p r < 2^256
 *
 * @param[out] r Product of @p x and 121666 modulo 2^255-19, reduced to 0 <= @p r < 2^256.
 * @param      x Factor to multiply with 121666.
 *
 * @remark @p r may be same as @p x.
 */
void occ_mod25519_mul121666(occ_mod25519 *r, const occ_mod25519 *x);

/**
 * modulo power
 *
 * @param[out] r
 * @parblock
 * - invert == 0: <pre>r = x^(2^252-3)  (sqrt(x)/x)</pre>
 * - invert == 1: <pre>r = x^(2^255-21) (r = 1/x)</pre>
 * @endparblock
 * @param      x Base
 * @param      invert Function to apply. See @p r.
 *
 * @remark @p r may be same as @p x
 */
void occ_mod25519_power(occ_mod25519 *r, const occ_mod25519 *x, int invert);

#endif
