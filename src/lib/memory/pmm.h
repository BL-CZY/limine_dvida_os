#ifndef MEMMAP_H
#define MEMMAP_H

#include "../../limine.h"
#include "../../lib/std/stdio.h"

#define PAGE_SIZE 0x1000ul

extern uint8_t *bit_map_start;
extern int bit_map_length;

void memmap_init(struct limine_memmap_response response);

#endif