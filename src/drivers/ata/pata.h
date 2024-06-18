#ifndef PATA_H
#define PATA_H

#include <stdint.h>
#include <stdbool.h>

#include "hal/structs/hal_storage_structs.h"

extern storage_device_t ata_primary_drive;
extern storage_device_t ata_secondary_drive;

void ata_drive_init();
int pio_read_sectors(storage_device_t *drive, int64_t index_lba, uint16_t sector_count, uint8_t *result);
int pio_write_sectors(storage_device_t *drive, int64_t index_lba, uint16_t sector_count, uint8_t *input);

#endif