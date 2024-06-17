#include "./rng.h"

rng_state_t default_rng_state;

// the state array is calculated by x[i] = f * (x[i-1] xor (x[i - 1] >> (w - 2))) + i
// suggest the seed to be 19650218UL
void init_rand_state(rng_state_t *state, uint32_t seed) {
    state->state_array[0] = seed;

    for(int i = 1; i < n; ++i) {
        state->state_array[i] = f * (state->state_array[i - 1] ^ (state->state_array[i - 1] >> (w - 2))) + i;
    }

    state->state_index = 0;
}

// the formula is x[k] = x[k - (n - m)] xor ((x[k - n] mask u | x[k - (n - 1) mask l])A)
// where x * A = x >> 1 if x & 0x1 == 0
//             = (x >> 1) xor a if x & 0x1 == 1
// where a is part of the coefficient set in MT19937
uint32_t random_uint32(rng_state_t *state) {
    int k = state->state_index;
    // as the state array is not infinitely long, we use circular indexing, so k - n is k
    // k - n; if(k < 0) {k + n;}

    int j = k - (n - 1);

    // if j < 0 
    if(j < 0) {
        j += n;
    }

    uint32_t x = ((state->state_array[k] & UMASK) | (state->state_array[j] & LMASK));

    uint32_t xA = x >> 1;
    if((x & 0x1) == 0x1) {
        xA ^= a;
    }

    j = k - n - m;
    if(j < 0) {
        j += n;
    }

    x = state->state_array[j] ^ xA; // update the state array
    state->state_array[k++] = x;

    if(k >= n) {
        k = 0;
    }
    
    state->state_index = k;

    // tempering to get the final result
    uint32_t y = x ^ (x >> u);
    y = y ^ ((y << s) & b);
    y = y ^ ((y << t) & c);
    uint32_t z = y ^ (y >> l);
    
    return z;
}