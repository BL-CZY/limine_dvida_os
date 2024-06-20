#include "./vector.h"

typedef struct vector {
   uint32_t element_size; // element size in byte
   void *start_addr;
   uint32_t element_amount;
   uint32_t max_element_amount;
} vector_t;

vector_t *new_vector(uint32_t data_size, vector_t *result) {
    // create an empty vector with initial size of 8
    result->element_size = data_size;
    result->max_element_amount = 8;
    result->element_amount = 0;

    // malloc the address
    result->start_addr = kmalloc(data_size * result->max_element_amount);
}

uint32_t vector_size(vector_t *vector) {
    return vector->element_amount;
}

void vector_push(void *data, vector_t *vector) {
    // if the size is too little reallocate the vector
    if(++vector->element_amount >= vector->max_element_amount) {
        vector->max_element_amount * 2;
        krealloc(vector->start_addr, vector->max_element_amount * vector->element_size);
    }

    // append the element
    for(uint32_t i = 0; i < vector->element_size; ++i)
    {
        ((uint8_t *)vector->start_addr)[(vector->element_size * vector->element_amount) + i] = ((uint8_t *)data)[i];
    }
}

void vector_pop(vector_t *vector) {
    // do nothing if the vector is empty
    if(vector->element_amount == 0) {
        return;
    }

    vector->element_amount--;

    // clear the data
    for(uint32_t i = 0; i < vector->element_size; ++i) {
        ((uint8_t *)vector->start_addr)[(vector->element_size * vector->element_amount) + i - 1] = 0;
    }
}