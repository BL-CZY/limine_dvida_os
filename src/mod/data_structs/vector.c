#include "./vector.h"
#include "mod/builtin_shell/stdio.h"

void new_vector(uint32_t data_size, vector_t *result) {
    // create an empty vector with initial size of 8
    result->element_size = data_size;
    result->max_element_amount = 8;
    result->count = 0;

    // malloc the address
    result->start_addr = kmalloc(data_size * result->max_element_amount);
    result->get = vector_get;
    result->set = vector_set;
    result->push = vector_push;
    result->pop = vector_pop;
    result->insert = vector_insert;
    result->remove = vector_remove;
}

void vector_get(vector_t *vector, uint32_t index, void *result) {
    // do nothing if the index is too large
    if(index >= vector->count) {
        return;
    }

    // put the value into result
    for(uint32_t i = 0; i < vector->element_size; ++i) {
        ((uint8_t *)result)[i] = ((uint8_t *)vector->start_addr)[(vector->element_size * index) + i];
    }
}

void vector_set(vector_t *vector, uint32_t index, void *input) {
    // do nothing if the index is too large
    if(index >= vector->count) {
        return;
    }

    // put the input into the vector
    for(uint32_t i = 0; i < vector->element_size; ++i) {
        ((uint8_t *)vector->start_addr)[(vector->element_size * index) + i] = ((uint8_t *)input)[i];
    }
}

void vector_push(vector_t *vector, void *data) {
    // if the size is too little reallocate the vector
    if(++vector->count > vector->max_element_amount) {
        vector->max_element_amount *= 2;
        vector->start_addr = krealloc(vector->start_addr, vector->max_element_amount * vector->element_size);
    }

    // append the element
    for(uint32_t i = 0; i < vector->element_size; ++i)
    {
        ((uint8_t *)vector->start_addr)[(vector->element_size * (vector->count - 1)) + i] = ((uint8_t *)data)[i];
    }
    // kprintf("%x ", ((uint8_t *)vector->start_addr)[(vector->element_size * vector->element_amount)]);
}

void vector_pop(vector_t *vector) {
    // do nothing if the vector is empty
    if(vector->count == 0) {
        return;
    }

    vector->count--;

    // clear the data
    for(uint32_t i = 0; i < vector->element_size; ++i) {
        ((uint8_t *)vector->start_addr)[(vector->element_size * vector->count) + i - 1] = 0;
    }
}

void vector_insert(vector_t *vector, void *data, uint32_t index) {
    // if the index is too large, do nothing
    if(index > vector->count) {
        return;
    }

    // if the size is too little reallocate the vector
    if(++vector->count > vector->max_element_amount) {
        vector->max_element_amount *= 2;
        vector->start_addr = krealloc(vector->start_addr, vector->max_element_amount * vector->element_size);
    }

    // reserve space for the inserted data
    for(uint32_t i = vector->count - 1; i >= index + 1; --i) {
        // copy the value of the previous element
        for(uint32_t j = 0; j < vector->element_size; ++j) {
            ((uint8_t *)vector->start_addr)[(vector->element_size * i) + j] = ((uint8_t *)vector->start_addr)[(vector->element_size * (i - 1)) + j];
        }
    }

    // copy the value into the reserved space
    for(uint32_t i = 0; i < vector->element_size; ++i)
    {
        ((uint8_t *)vector->start_addr)[(vector->element_size * index) + i] = ((uint8_t *)data)[i];
    }
}

void vector_remove(vector_t *vector, uint32_t index) {
    // if the index is too large, do nothing
    if(index >= vector->count) {
        return;
    }

    vector->count--;
    if(index == vector->count) {
        vector_pop(vector);
        return;
    }

    // move the values to one before
    for(uint32_t i = index; i < vector->count; ++i) {
        // copy the value of the next element
        for(uint32_t j = 0; j < vector->element_size; ++j) {
            ((uint8_t *)vector->start_addr)[(vector->element_size * i) + j] = ((uint8_t *)vector->start_addr)[(vector->element_size * (i + 1)) + j];
        }
    }

    // remove the last data
    for(uint32_t i = 0; i < vector->element_size; ++i) {
        ((uint8_t *)vector->start_addr)[(vector->element_size * vector->count) + i] = 0;
    }
}