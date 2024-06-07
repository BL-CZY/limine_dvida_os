#ifndef MEMMAP_H
#define MEMMAP_H

#include "../../limine.h"
#include "../builtin_shell/stdio.h"
#include <stdbool.h>
#include "../other_utils/handle_utils.h"
#include "../../limine.h"

#define PAGE_SIZE 0x1000ul

extern uint8_t *bit_map_start;
extern uint64_t bit_map_length;

void pmm_init(struct limine_memmap_response *response, struct limine_hhdm_response *hhdm);
void *allocate_continuous_pages(uint64_t num);
void free_continuous_pages(void *start, uint64_t length);

bool bit_map_get_bit(uint64_t num);
void bit_map_allocate_bit(uint64_t bit);
void bit_map_free_bit(uint64_t bit);

void fill_map(struct limine_memmap_response *response);

#endif