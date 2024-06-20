#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>

#include "mod/kheap/kheap_utils.h"

typedef struct vector {
   uint32_t element_size; // element size in byte
   void *start_addr;
   uint32_t element_amount;
   uint32_t max_element_amount;
} vector_t;

void new_vector(uint32_t data_size, vector_t *result);
void vector_get(vector_t *vector, uint32_t index, void *result);
void vector_set(vector_t *vector, uint32_t index, void *input);
void vector_push(void *data, vector_t *vector);
void vector_pop(vector_t *vector);
void vector_insert(vector_t *vector, void *data, uint32_t index);
void vector_remove(vector_t *vector, uint32_t index);

#endif