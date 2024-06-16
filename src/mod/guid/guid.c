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
