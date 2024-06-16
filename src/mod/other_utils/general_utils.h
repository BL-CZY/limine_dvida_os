#ifndef GENERAL_UTILS
#define GENERAL_UTILS

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "drivers/partition/gpt.h"

uint8_t big_endian_to_uint8(uint8_t *buffer);
uint16_t big_endian_to_uint16(uint8_t *buffer);
uint32_t big_endian_to_uint32(uint8_t *buffer);
uint64_t big_endian_to_uint64(uint8_t *buffer);

uint8_t little_endian_to_uint8(uint8_t *buffer);
uint16_t little_endian_to_uint16(uint8_t *buffer);
uint32_t little_endian_to_uint32(uint8_t *buffer);
uint64_t little_endian_to_uint64(uint8_t *buffer);

// returns length if the two buffers are same and returns the first index of difference if the two buffers are different
size_t compare_buffer(uint8_t *buffer1, uint8_t *buffer2, size_t length);
bool are_buffers_same(uint8_t *buffer1, uint8_t *buffer2, size_t length);
void extend_buffer(uint8_t *buffer1, uint8_t *buffer2, size_t start, size_t length);

void uint8_to_big_endian(uint8_t input, uint8_t *result);
void uint16_to_big_endian(uint16_t input, uint8_t *result);
void uint32_to_big_endian(uint32_t input, uint8_t *result);
void uint64_to_big_endian(uint64_t input, uint8_t *result);

void uint8_to_little_endian(uint8_t input, uint8_t *result);
void uint16_to_little_endian(uint16_t input, uint8_t *result);
void uint32_to_little_endian(uint32_t input, uint8_t *result);
void uint64_to_little_endian(uint64_t input, uint8_t *result);

#endif