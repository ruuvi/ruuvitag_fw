//
//  Optimized Crypto Library.
//  Copyright 2016 Oberon microsystems, Inc.
//

#ifndef OCC_ECDSA_P256_H
#define OCC_ECDSA_P256_H

#include <stdint.h>


// ecdsa P-256 signature key pair generation
// pk[64]: the generated public key
// sk[32]: the private key
// returns 0 if sk is a legal secret key
int occ_ecdsa_p256_public_key(uint8_t pk[64], const uint8_t sk[32]);

// ecdsa P-256 signature generate
// sig[64]: the generated signature
// m[mlen]: the input message
// sk[32]:  the secret key
// ek[32]:  the session key
// returns 0 if ek is a valid session key
int occ_ecdsa_p256_sign(uint8_t sig[64], const uint8_t *m, uint32_t mlen, const uint8_t sk[32], const uint8_t ek[32]);

// ecdsa P-256 signature verification
// sig[64]: the input signature
// m[mlen]: the input message
// pk[64]:  the public key
// returns 0 if signature is valid, -1 otherwise
int occ_ecdsa_p256_verify(const uint8_t sig[64], const uint8_t *m, uint32_t mlen, const uint8_t pk[64]);


#endif
