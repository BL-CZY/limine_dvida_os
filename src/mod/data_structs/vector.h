#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>

#include "mod/kheap/kheap_utils.h"

typedef struct vector_body {
   uint32_t element_size; // element size in byte
   void *start_addr;
   uint32_t element_amount;
   uint32_t max_element_amount;
} vector_body_t;

uint32_t vector_size(vector_body_t *vector);
void vector_get(vector_body_t *vector, uint32_t index, void *result);
void vector_set(vector_body_t *vector, uint32_t index, void *input);
void vector_push(void *data, vector_body_t *vector);
void vector_pop(vector_body_t *vector);
void vector_insert(vector_body_t *vector, void *data, uint32_t index);
void vector_remove(vector_body_t *vector, uint32_t index);

typedef struct vector {
   vector_body_t body;
   uint32_t (*size)(vector_body_t *vector_body);
   void (*get)(vector_body_t *vector_body, uint32_t index, void *result);
   void (*set)(vector_body_t *vector_body, uint32_t index, void *input);
   void (*push)(void *data, vector_body_t *vector);
   void (*pop)(vector_body_t *vector);
   void (*insert)(vector_body_t *vector_body, void *data, uint32_t index);
   void (*remove)(vector_body_t *vector, uint32_t index);
} vector_t;

void new_vector(uint32_t data_size, vector_t *result);

#endif