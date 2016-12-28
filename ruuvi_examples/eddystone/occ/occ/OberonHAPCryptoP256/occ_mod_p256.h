//
//  Optimized Crypto Library.
//  Copyright 2016 Oberon microsystems, Inc.
//
 
#ifndef OCC_MOD_P256_H
#define OCC_MOD_P256_H

#include <stdint.h>


// modulo P-256 number
typedef struct 
{
  uint32_t w[8]; // little endian
}
occ_mod_p256;

// convert bytes to modulo number
void occ_mod_p256_fromBytes(occ_mod_p256 *r, const uint8_t x[32]);

// convert modulo number to bytes
void occ_mod_p256_toBytes(uint8_t r[32], const occ_mod_p256 *x);

// r = val
void occ_mod_p256_init(occ_mod_p256 *r, uint32_t val);

// conditional negate modulo P-256
// r = b ? -a : a
// condition b must be 0 or 1
// r may be same as a
void occ_mod_p256_cneg(occ_mod_p256 *r, const occ_mod_p256 *a, int b);

// get sign = lsb of fully reduced number
// returns sign bit of x
int occ_mod_p256_getsign(const occ_mod_p256 *x);

// modulo add
// r = (x + y) mod P-256
// reduced to: 0 <= r < P
// r may be same as x or y
void occ_mod_p256_add(occ_mod_p256 *r, const occ_mod_p256 *x, const occ_mod_p256 *y);

// modulo subtract
// r = (x - y) mod P-256
// reduced to: 0 <= r < P
// r may be same as x or y
void occ_mod_p256_sub(occ_mod_p256 *r, const occ_mod_p256 *x, const occ_mod_p256 *y);

// modulo multiply
// r = (x * y) mod P-256
// reduced to: 0 <= r < P
// r may be same as x or y
void occ_mod_p256_mul(occ_mod_p256 *r, const occ_mod_p256 *x, const occ_mod_p256 *y);

// modulo square
// r = x^2 mod P-256
// reduced to: 0 <= r < P
// r may be same as x
void occ_mod_p256_square(occ_mod_p256 *r, const occ_mod_p256 *x);

// modulo invert
// r = x^(p-2) = 1/x
// r may be same as x
void occ_mod_p256_invert(occ_mod_p256 *r, const occ_mod_p256 *x);

// modulo square root
// r = x^(p+1)/4 = sqrt(x)
// r may be same as x
void occ_mod_p256_sqrt(occ_mod_p256 *r, const occ_mod_p256 *x);

#endif
