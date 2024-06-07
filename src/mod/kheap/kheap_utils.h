//implemented in mod/dyn_mem/kheap.c

#ifndef LIBKHEAP_UTILS_H
#define LIBKHEAP_UTILS_H

#include <stddef.h>

void *kmalloc(size_t size);
void kfree(void *target);
void *krealloc(void *target, size_t new_size);

void kheap_init();

#endif