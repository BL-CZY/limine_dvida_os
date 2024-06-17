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

void guid_to_buffer(guid_t *uid, uint8_t *result) {
    for(int i = 0; i < 4; ++i) {
        result[i] = uid->data1[3 - i];
    }

    for(int i = 0; i < 2; ++i) {
        result[4 + i] = uid->data2[1 - i];
    }

    for(int i = 0; i < 2; ++i) {
        result[6 + i] = uid->data3[1 - i];
    }

    for(int i = 0; i < 2; ++i) {
        result[8 + i] = uid->data4[i];
    }

    for(int i = 0; i < 6; ++i) {
        result[10 + i] = uid->data5[i];
    }
}

bool are_guid_same(guid_t *guid1, guid_t *guid2) {
    uint8_t guid1_buf[16], guid2_buf[16];
    guid_to_buffer(guid1, guid1_buf);
    guid_to_buffer(guid2, guid2_buf);

    for(int i = 0; i < 16; ++i) {
        if(guid1_buf[i] != guid2_buf[i]) {
            return false;
        }
    }

    return true;
}

void cpy_guid(guid_t *input, guid_t *target) {
    for(int i = 0; i < 4; ++i) {
        target->data1[i] = input->data1[i];
    }

    for(int i = 0; i < 2; ++i) {
        target->data2[i] = input->data2[i];
    }

    for(int i = 0; i < 2; ++i) {
        target->data3[i] = input->data3[i];
    }

    for(int i = 0; i < 2; ++i) {
        target->data4[i] = input->data4[i];
    }

    for(int i = 0; i < 6; ++i) {
        target->data5[i] = input->data5[i];
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