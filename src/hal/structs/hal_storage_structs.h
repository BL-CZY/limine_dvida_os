#ifndef HAL_STORAGE_STRUCTS_H
#define HAL_STORAGE_STRUCTS_H

#include <stdint.h>
#include "./hal_fs_structs.h"

#define DISKIO_NO_ERR 0
#define DISKIO_ERR_DEVICE_UNIDENTIFIED 1
#define DISKIO_ERR_DEVICE_NOT_READY 2
#define DISKIO_ERR_DEVICE_TIMEOUT 3
#define DISKIO_ERR_FLUSH_CACHE_FAILED 4
#define DISKIO_ERR_SECTOR_DOESNT_EXIST 5

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

    uint8_t serial[20];

    gpt_efi_header_t device_gpt_header;
    gpt_table_t device_gpt_table;
} storage_device_t;

#endif