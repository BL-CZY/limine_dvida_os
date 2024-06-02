#include "../../lib/utils/general_utils.h"

uint8_t buffer_to_uint8(uint8_t *buffer) {
    return (uint8_t)buffer[0];
}

uint16_t buffer_to_uint16(uint8_t *buffer) {
    return ((uint16_t)buffer[0]) << 8 | (uint16_t)buffer[1];
}

uint32_t buffer_to_uint32(uint8_t *buffer) {
    return ((uint32_t)buffer_to_uint16(buffer)) << 16 | (uint32_t)buffer_to_uint16(buffer + 2);
}

uint64_t buffer_to_uint64(uint8_t *buffer) {
    return ((uint64_t)buffer_to_uint32(buffer)) << 32 | (uint32_t)buffer_to_uint32(buffer + 4);
}

size_t compare_buffer(uint8_t *buffer1, uint8_t *buffer2, size_t length) {
    for(size_t i = 0; i < length; ++i) {
        if(buffer1[i] != buffer2[i]) {
            return i;
        }
    }
    return length;
}

bool are_buffers_same(uint8_t *buffer1, uint8_t *buffer2, size_t length) {
    for(size_t i = 0; i < length; ++i) {
        if(buffer1[i] != buffer2[i]) {
            return false;
        }
    }
    return true;
}

void extend_buffer(uint8_t *buffer1, uint8_t *buffer2, size_t start, size_t length) {
    for(size_t i = start; i < start + length; ++i) {
        buffer1[i] = buffer2[i - start];
    }
}

void *uint8_to_buffer(uint8_t input, uint8_t *result) {
    result[0] = input;
    return result;
}

void *uint16_to_buffer(uint16_t input, uint8_t *result) {
    result[0] = (uint8_t)(input >> 8);
    result[1] = (uint8_t)(input);
    return result;
}

void *uint32_to_buffer(uint32_t input) {
}

void *uint64_to_buffer(uint64_t input);