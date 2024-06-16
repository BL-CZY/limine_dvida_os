#ifndef GUID_H
#define GUID_H

#include <stdint.h>

//* data 1, 2, and 3 are in little endian, and data 4 and 5 are in big endian
typedef struct guid {
    uint8_t data1[4];
    uint8_t data2[2];
    uint8_t data3[2];
    uint8_t data4[2];
    uint8_t data5[6];
} guid_t;

void buffer_to_guid(uint8_t *buffer, guid_t *result);

#endif