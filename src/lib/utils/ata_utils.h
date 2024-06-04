#ifndef ATA_UTILS_H
#define ATA_UTILS_H

#include <stdint.h>

void ata_drive_init();
int pio_read_sector(uint64_t lba, uint8_t *result);
int pio_write_sector(uint64_t lba, uint8_t *input);

#endif