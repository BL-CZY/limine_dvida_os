#ifndef HAL_STORAGE_H
#define HAL_STORAGE_H

#include <stdbool.h>

#include "hal/structs/hal_storage_structs.h"
#include "drivers/ata/pata.h"

int read_sectors(storage_device_t *drive, int64_t index_lba, uint16_t sector_count, uint8_t *result);
int write_sectors(storage_device_t *drive, int64_t index_lba, uint16_t sector_count, uint8_t *input);

#endif