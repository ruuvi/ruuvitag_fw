//
//  Optimized Crypto Library for OberonHAP.
//  Copyright 2016 Oberon microsystems, Inc.
//
 
#ifndef OCC_SRP_MATH_H
#define OCC_SRP_MATH_H

#include <stdint.h>


/**
 * scaled numbers for Montgomery multiplication
 */
typedef struct 
{
    uint32_t w[96]; /**< `x * 2^1536 modulo p3072`, little endian */
}
occ_srp_mg;


/**
 * convert bytes to Montgomery scaled number
 *
 * @param[out] y Conversion result.
 * @param      x Bytes to convert.
 *
 * @remark @p y may be same as @p x.
 */
void occ_srp_fromBytes(occ_srp_mg* y, const uint8_t x[384]);

/**
 * convert Montgomery scaled number to bytes
 *
 * @param[out] y Conversion result.
 * @param      x Montgomery scaled number to convert.
 */
void occ_srp_toBytes(uint8_t y[384], const occ_srp_mg* x);

/**
 * `r = a+b mod p3072`
 *
 * @remark @p r may be same as @p a or @p b.
 */
void occ_srp_addmod3072(occ_srp_mg* r, const occ_srp_mg* a, const occ_srp_mg* b);

/**
 * `r = a-b mod p3072`
 *
 * @remark @p r may be same as @p a or @p b.
 */
void occ_srp_submod3072(occ_srp_mg* r, const occ_srp_mg* a, const occ_srp_mg* b);

/**
 * `r = r+inc mod p3072`
 *
 * @returns 1 If r == 0.
 * @returns 0 Otherwise.
 */
int occ_srp_incmod3072(occ_srp_mg* r, int inc);

/**
 * `r = a*b/2^1536 mod p3072`
 *
 * @remark @p r may be same as @p a or @p b.
 */
void occ_srp_mulmod3072(occ_srp_mg* r, const occ_srp_mg* a, const occ_srp_mg* b);

/**
 * `r = a^b mod p3072`
 */
void occ_srp_expmod3072(occ_srp_mg* r, const occ_srp_mg* a, const uint8_t* b, unsigned int blen);

/**
 * `r = 5^b mod p3072`
 */
void occ_srp_base_expmod3072(occ_srp_mg* r, const uint8_t* b, unsigned int blen);


#endif
