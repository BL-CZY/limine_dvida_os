//implemented in mod/utils/mem_utils.c

#ifndef MEM_UTILS_H
#define MEM_UTILS_H

#include <stddef.h>
#include <stdint.h>

void *memcpy(void *dest, const void *src, size_t length);
void *memset(void *dest, int value, size_t length);
void *memmove(void *dest, const void *src, size_t length);
int memcmp(const void *s1, const void *s2, size_t length);

#endif