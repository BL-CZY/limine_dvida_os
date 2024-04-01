#include "../../lib/utils/math_utils.h"

uint64_t round_up(uint64_t first, uint64_t second) {
    return first/second + (first%second != 0);
}