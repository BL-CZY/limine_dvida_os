#ifndef CRC32_H
#define CRC32_H

#include <stddef.h>
#include <stdint.h>

void initialize_crc32();

void partial_crc(uint32_t *iCRC, uint8_t *sData, size_t iDataLength);
unsigned long full_crc(uint8_t *sData, uint64_t ulDataLength);

unsigned int reflect(uint32_t iReflect, char cChar);

#endif