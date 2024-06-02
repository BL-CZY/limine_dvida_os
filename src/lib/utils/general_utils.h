#ifndef GENERAL_UTILS
#define GENERAL_UTILS

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

uint8_t buffer_to_uint8(uint8_t *buffer);
uint16_t buffer_to_uint16(uint8_t *buffer);
uint32_t buffer_to_uint32(uint8_t *buffer);
uint64_t buffer_to_uint64(uint8_t *buffer);

// returns length if the two buffers are same and returns the first index of difference if the two buffers are different
size_t compare_buffer(uint8_t *buffer1, uint8_t *buffer2, size_t length);
bool are_buffers_same(uint8_t *buffer1, uint8_t *buffer2, size_t length);
void extend_buffer(uint8_t *buffer1, uint8_t *buffer2, size_t start, size_t length);

void uint8_to_buffer(uint8_t input, uint8_t *result);
void uint16_to_buffer(uint16_t input, uint8_t *result);
void uint32_to_buffer(uint32_t input, uint8_t *result);
void uint64_to_buffer(uint64_t input, uint8_t *result);

#endif