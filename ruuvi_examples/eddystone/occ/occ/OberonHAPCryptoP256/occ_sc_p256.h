//
//  Optimized Crypto Library.
//  Copyright 2016 Oberon microsystems, Inc.
//

#ifndef OCC_SC_P256_H
#define OCC_SC_P256_H

#include <stdint.h>
#include "occ_mod_p256.h"


// P-256 scalar modulo group order
typedef struct 
{
    uint32_t w[8]; // little endian
}
occ_sc_p256;


// 32 byte value to scalar
void occ_sc_p256_from32bytes(occ_sc_p256 *r, const uint8_t x[32]);

// modulo p256 value to scalar
void occ_sc_p256_from_mod256(occ_sc_p256 *r, const occ_mod_p256 *x);

// scalar to 32 byte value
void occ_sc_p256_to32bytes(uint8_t r[32], const occ_sc_p256 *x);

// add scalars
void occ_sc_p256_add(occ_sc_p256 *r, const occ_sc_p256 *x, const occ_sc_p256 *y);

// multiply scalars
void occ_sc_p256_mul(occ_sc_p256 *r, const occ_sc_p256 *x, const occ_sc_p256 *y);

// modulo inversion
// r = x^(q-2) = 1/x
void occ_sc_p256_invert(occ_sc_p256 *r, const occ_sc_p256 *x);


#endif
