#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdbool.h>
#include "../../lib/handlers/handle_utils.h"
#include "../../limine.h"

bool bit_map_get_bit(uint64_t num);
void bit_map_allocate_bit(uint64_t bit);
void bit_map_free_bit(uint64_t bit);

void fill_map(struct limine_memmap_response *response);

#endif