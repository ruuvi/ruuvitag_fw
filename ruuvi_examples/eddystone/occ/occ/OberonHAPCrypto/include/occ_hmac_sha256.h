//
//  Optimized Crypto Library for OberonHAP.
//  Copyright 2016 Oberon microsystems, Inc.
//

/**@file
 * HMAC-SHA256 is an algorithm for message authentication using the
 * cryptographic hash function SHA256 and a reusable secret key. Users in
 * possession of the key can verify the integrity and authenticity of the
 * message.
 *
 * @see [RFC 2104 - HMAC: Keyed-Hashing for Message Authentication](http://tools.ietf.org/html/rfc2104)
 */
 
#ifndef OCC_HMAC_SHA256_H
#define OCC_HMAC_SHA256_H

#include <stdint.h>
#include <stddef.h>


/**
 * Maximum key length.
 */
#define occ_hmac_sha256_KEY_BYTES_MAX (64)

/**
 * Length of the authenticator.
 */
#define occ_hmac_sha256_BYTES (32)


/**
 * HMAC-SHA256 algorithm.
 *
 * The input message @p in is authenticated using the key @p k. The computed
 * authenticator is put into @p r. To verify the authenticator, the recipient
 * needs to recompute the HMAC authenticator and can then compare it with the
 * received authenticator.
 *
 * **Example**
 * @include occ_hmac_sha256.c
 *
 * @param[out] r       HMAC output.
 * @param      key     HMAC key.
 * @param      key_len Length of @p key. 0 <= @p key_len <= @c occ_hmac_sha256_KEY_BYTES_MAX.
 * @param      in      Input data.
 * @param      in_len  Length of @p in.
 */
void occ_hmac_sha256(uint8_t r[occ_hmac_sha256_BYTES],
                     const uint8_t* key, size_t key_len,
                     const uint8_t* in, size_t in_len);

#endif
