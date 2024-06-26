#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>

#include "mod/kheap/kheap_utils.h"

typedef struct vector {
   uint32_t element_size; // element size in byte
   void *start_addr;
   uint32_t count;
   uint32_t max_element_amount;
   void (*get)(void *vector, uint32_t index, void *result);
   void (*set)(void *vector, uint32_t index, void *input);
   void (*push)(void *vector, void *data);
   void (*pop)(void *vector);
   void (*insert)(void *vector, void *data, uint32_t index);
   void (*remove)(void *vector, uint32_t index);
} vector_t;

void vector_get(vector_t *vector, uint32_t index, void *result);
void vector_set(vector_t *vector, uint32_t index, void *input);
void vector_push(vector_t *vector, void *data);
void vector_pop(vector_t *vector);
void vector_insert(vector_t *vector, void *data, uint32_t index);
void vector_remove(vector_t *vector, uint32_t index);

void new_vector(uint32_t data_size, vector_t *result);

#endif