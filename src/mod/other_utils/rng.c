#include "./rng.h"

static uint32_t state_array[n];
int state_index;

// the state arry is calculated by x[i] = f * (x[i-1] xor (x[i - 1] >> (w - 2))) + i
// suggest the seed to be 19650218UL
void init_rand_state(uint32_t seed) {

}