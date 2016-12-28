//
//  Optimized Crypto Library for OberonHAP.
//  Copyright 2016 Oberon microsystems, Inc.
//

#ifndef OCC_GE25519_H
#define OCC_GE25519_H

#include "occ_sc25519.h"
#include "occ_mod25519.h"


/**
 * projective coordinates
 */
typedef struct {
    /**@{
     * point = `(x/z, y/z)`
     */
    occ_mod25519 x;
    occ_mod25519 y;
    occ_mod25519 z;
    /**@}*/
} occ_ge25519_p2;

/**
 * extened coordinates
 */
typedef struct {
    /**@{
     * point = `(x/z, y/z)`
     * @remark `x*y = t*z`
     */
    occ_mod25519 x;
    occ_mod25519 y;
    occ_mod25519 z;
    occ_mod25519 t;
    /**@}*/
} occ_ge25519_p3;

/**
 * completed coordinates
 */
typedef struct {
    /**@{
     * point = `(e/g, h/f)`
     */
    occ_mod25519 e;
    occ_mod25519 f;
    occ_mod25519 g;
    occ_mod25519 h;
    /**@}*/
} occ_ge25519_p1p1;

/**
 * affine coordinates
 */
typedef struct {
    /**@{
     * point = `(x, y)`
     */
    occ_mod25519 x;
    occ_mod25519 y;
    /**@}*/
} occ_ge25519_aff;


/**
 * `r = -unpack(p)`
 *
 * @param[out] r `-unpack(p)`.
 * @param      p Packed curve point.
 *
 * @returns 0  If p is a legal curve point.
 * @returns -1 Otherwise
 */
int occ_ge25519_unpackneg(occ_ge25519_p3 *r, const uint8_t p[32]);

/**
 * `r = pack(p)`
 *
 * @param[out] r `pack(p)`.
 * @param      p Curve point.
 */
void occ_ge25519_pack(uint8_t r[32], const occ_ge25519_p2 *p);

/**
 * `r = s1 * p1 + s2 * basePoint`
 */
void occ_ge25519_double_scalarmult_vartime(occ_ge25519_p2 *r,
                                           const occ_ge25519_p3 *p1,
                                           const occ_sc25519 *s1,
                                           const occ_sc25519 *s2);

/**
 * `r = s * basePoint`
 */
void occ_ge25519_scalarmult_base(occ_ge25519_p2 *r, const occ_sc25519 *s);

#endif
