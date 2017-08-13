#ifndef RNG_H
#define RNG_H
#include <stdint.h>

/*
 *  Initialises RNG Driver
 */
uint32_t init_rng(void);

/*
 * Return 4 random bytes / uint32_t. 
 * Uses up bytes from a pool of max 32 bytes, i.e. 8 calls
 * Blocking function, will take relatively long time (~0.5ms, not guaranteed, varies) if bytes are not availalble
 */
uint32_t random(void);

/*
 * Returns number of available random bytes. One call to random() copnsumes 4 bytes.
 * Random bytes are automatically replenished over time
 */
uint8_t randoms_available(void);
#endif
