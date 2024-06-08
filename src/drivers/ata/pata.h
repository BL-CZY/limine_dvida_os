#ifndef PATA_H
#define PATA_H

#include <stdint.h>
#include <stdbool.h>

typedef struct ata_drive {
    bool identified;
    uint16_t base_port;
    bool is_lba48_supported;
    uint32_t lba28_sector_count;
    uint64_t lba48_sector_count;
    uint8_t serial[20];
} ata_drive_t;

extern ata_drive_t ata_primary_drive;
extern ata_drive_t ata_secondary_drive;

void ata_drive_init();
int pio_read_sector(ata_drive_t *drive, int64_t index_lba, uint16_t sector_count, uint8_t *result);
int pio_write_sector(ata_drive_t *drive, int64_t index_lba, uint16_t sector_count, uint8_t *input);

#endif