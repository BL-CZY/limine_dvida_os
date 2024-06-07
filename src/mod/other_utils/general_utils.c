#include "general_utils.h"

uint8_t big_endian_to_uint8(uint8_t *buffer) {
    return (uint8_t)buffer[0];
}

uint16_t big_endian_to_uint16(uint8_t *buffer) {
    return ((uint16_t)buffer[0]) << 8 | (uint16_t)buffer[1];
}

uint32_t big_endian_to_uint32(uint8_t *buffer) {
    return ((uint32_t)big_endian_to_uint16(buffer)) << 16 | (uint32_t)big_endian_to_uint16(buffer + 2);
}

uint64_t big_endian_to_uint64(uint8_t *buffer) {
    return ((uint64_t)big_endian_to_uint32(buffer)) << 32 | (uint32_t)big_endian_to_uint32(buffer + 4);
}

uint8_t little_endian_to_uint8(uint8_t *buffer) {
    return (uint8_t)buffer[0];
}

uint16_t little_endian_to_uint16(uint8_t *buffer) {
    return (uint16_t)buffer[0] | ((uint16_t)buffer[1]) << 8;
}

uint32_t little_endian_to_uint32(uint8_t *buffer) {
    return (uint32_t)little_endian_to_uint16(buffer) | ((uint32_t)little_endian_to_uint16(buffer + 2)) << 16;
}

uint64_t little_endian_to_uint64(uint8_t *buffer) {
    return (uint64_t)little_endian_to_uint32(buffer) | ((uint64_t)little_endian_to_uint32(buffer + 4)) << 32;
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

void uint8_to_big_endian(uint8_t input, uint8_t *result) {
    result[0] = input;
}

void uint16_to_big_endian(uint16_t input, uint8_t *result) {
    result[0] = (uint8_t)(input >> 8);
    result[1] = (uint8_t)(input);
}

void uint32_to_big_endian(uint32_t input, uint8_t *result) {
    uint8_t buffer[2];
    uint16_to_big_endian((uint16_t)(input >> 16), result);
    uint16_to_big_endian((uint16_t)(input), buffer);
    extend_buffer(result, buffer, 2, 2);
}

void uint64_to_big_endian(uint64_t input, uint8_t *result) {
    uint8_t buffer[4];
    uint32_to_big_endian((uint32_t)(input >> 32), result);
    uint32_to_big_endian((uint32_t)(input), buffer);
    extend_buffer(result, buffer, 4, 4);
}

void uint8_to_little_endian(uint8_t input, uint8_t *result) {
    result[0] = input;
}

void uint16_to_little_endian(uint16_t input, uint8_t *result) {
    result[0] = (uint8_t)input;
    result[1] = (uint8_t)(input >> 8);
}

void uint32_to_little_endian(uint32_t input, uint8_t *result) {
    uint8_t buffer[2];
    uint16_to_little_endian((uint16_t)input, result);
    uint16_to_little_endian((uint16_t)(input >> 16), buffer);
    extend_buffer(result, buffer, 2, 2);
}

void uint64_to_little_endian(uint64_t input, uint8_t *result) {
    uint8_t buffer[4];
    uint32_to_little_endian((uint32_t)input, result);
    uint32_to_little_endian((uint32_t)(input >> 32), buffer);
    extend_buffer(result, buffer, 4, 4);
}