#ifndef GPT_H
#define GPT_H

#include "../drives/pata.h"

typedef struct gpt_table_entry {
    uint8_t partition_type_guid[16];
    uint8_t unique_partition_guid[16];
    uint64_t start_lba;
    uint64_t end_lba;
    uint8_t attributes[8];
    char name[72];
} gpt_table_entry_t;

typedef struct gpt_table {
    gpt_table_entry_t *entries;
} gpt_table_t;

typedef struct gpt_efi_header {
    uint8_t header[8];
    uint8_t revision[4];
    uint32_t header_size; //in little endian
    uint32_t check_sum;
    uint32_t reserved;
    uint64_t header_lba_address;
    uint64_t backup_header_lba_address;
    uint64_t first_usable_block;
    uint64_t last_usable_block;
    uint8_t guid[16];
    uint64_t partition_array_start_lba;
    uint32_t entry_num;
    uint32_t entry_size;
    uint32_t crc32;
} gpt_efi_header_t;

#endif
