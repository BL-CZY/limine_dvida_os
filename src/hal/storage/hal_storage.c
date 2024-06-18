#include "./hal_storage.h"

// 1: drive not identified
int read_sectors(storage_device_t *drive, int64_t index_lba, uint16_t sector_count, uint8_t *result) {
    if(!drive->identified) {
        return 1;
    }

    if(drive->drive_device_type == PATA) {
        if(drive->dma_available) {
            // TODO use DMA
        } else {
            return pio_read_sectors(drive, index_lba, sector_count, result);
        }
    }

    return 0;
}

// 1: drive not identified
int write_sectors(storage_device_t *drive, int64_t index_lba, uint16_t sector_count, uint8_t *input) {
    if(!drive->identified) {
        return 1;
    }

    if(drive->drive_device_type == PATA) {
        if(drive->dma_available) {
            // TODO use DMA
        } else {
            return pio_write_sectors(drive, index_lba, sector_count, input);
        }
    }

    return 0;
}