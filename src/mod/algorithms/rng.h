#ifndef RNG_H
#define RNG_H

#include <stdint.h>

// MT19937 parameters for the Mersenne Twister
#define n 624
#define m 397
#define w 32
#define r 31
#define UMASK (0xffffffffUL << r)
#define LMASK (0xffffffffUL >> (w-r))
#define a 0x9908b0dfUL
#define u 11
#define s 7
#define t 15
#define l 18
#define b 0x9d2c5680UL
#define c 0xefc60000UL
#define f 1812433253UL

typedef struct rng_state {
    uint32_t state_array[n];
    int state_index;
} rng_state_t;

extern rng_state_t default_rng_state;

void init_rand_state(rng_state_t *state, uint32_t seed);
uint32_t random_uint32(rng_state_t *state);

#endif