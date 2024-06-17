#include "./guid.h"

void buffer_to_guid(uint8_t *buffer, guid_t *result) {
    for(int i = 0; i < 4; ++i) {
        result->data1[i] = buffer[3 - i];
    }

    for(int i = 0; i < 2; ++i) {
        result->data2[i] = buffer[5 - i];
    }

    for(int i = 0; i < 2; ++i) {
        result->data3[i] = buffer[7 - i];
    }

    for(int i = 0; i < 2; ++i) {
        result->data4[i] = buffer[8 + i];
    }

    for(int i = 0; i < 6; ++i) {
        result->data5[i] = buffer[10 + i];
    }
}

void new_guid(guid_t *result) {
    uint32_to_little_endian(random_uint32(&default_rng_state), result->data1);

    uint32_t temp = random_uint32(&default_rng_state);
    uint16_to_little_endian((uint16_t)(temp >> 16), result->data2);
    uint16_to_little_endian((uint16_t)(temp), result->data3);

    temp = random_uint32(&default_rng_state);
    uint32_t temp1 = random_uint32(&default_rng_state);
    uint16_to_big_endian((uint16_t)(temp >> 16), result->data4);
    uint64_t temp2 = (((uint64_t)temp) << 32) | ((uint64_t)temp1);

    for(int i = 0; i < 6; ++i) {
        result->data5[i] = (uint8_t)((temp2 >> (i * 8)) & 0xFF);
    }
}