//implemented in mod/utils/mem_utils.c
//implemented in mod/dyn_mem/kheap.c

#ifndef MEM_UTILS_H
#define MEM_UTILS_H

#include <stddef.h>
#include <stdint.h>

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

void *kmalloc(size_t size);
void kfree(void *target);
void *krealloc(void *target, size_t new_size);

void kheap_init();

#endif