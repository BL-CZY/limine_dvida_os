#ifndef HAL_STORAGE_STRUCTS_H
#define HAL_STORAGE_STRUCTS_H

#include <stdint.h>
#include "mod/algorithms/guid.h"

#define DISKIO_NO_ERR 0
#define DISKIO_ERR_DEVICE_UNIDENTIFIED 1
#define DISKIO_ERR_DEVICE_NOT_READY 2
#define DISKIO_ERR_DEVICE_TIMEOUT 3
#define DISKIO_ERR_FLUSH_CACHE_FAILED 4
#define DISKIO_ERR_SECTOR_DOESNT_EXIST 5

typedef struct gpt_table_entry {
    guid_t partition_type_guid;
    guid_t unique_partition_guid;
    uint64_t start_lba;
    uint64_t end_lba;
    uint64_t flags;
    uint16_t utf16_name[36];
} gpt_table_entry_t;

typedef struct gpt_table {
    uint16_t entry_count;
    gpt_table_entry_t entries[128]; // the maximum number should be 128
} gpt_table_t;

typedef struct gpt_efi_header {
    uint8_t signature[8];
    uint8_t revision[4];
    uint32_t header_size; //in little endian
    uint32_t header_crc32;
    uint32_t reserved;
    uint64_t header_lba_address;
    uint64_t backup_header_lba_address;
    uint64_t first_usable_block_lba;
    uint64_t last_usable_block_lba;
    guid_t disk_guid;
    uint64_t partition_array_start_lba;
    uint32_t entry_num;
    uint32_t entry_size;
    uint32_t array_crc32;
} gpt_efi_header_t;

enum device_type {
    PATA,
    SATA,
    NVME,
};

typedef struct storage_device {
    uint16_t base_port;
    bool identified;

    enum device_type drive_device_type;
    bool dma_available;

    bool is_lba48_supported;
    uint32_t lba28_sector_count;
    uint64_t lba48_sector_count;

    uint16_t num_sectors_per_track;

    uint8_t serial[20];

    gpt_efi_header_t device_gpt_header;
    gpt_table_t device_gpt_table;
} storage_device_t;

typedef struct mounted_partition {
    storage_device_t *drive;
    gpt_table_entry_t *mounted_entry;
} mounted_partition_t;

#endif