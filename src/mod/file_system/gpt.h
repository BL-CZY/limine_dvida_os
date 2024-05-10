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

typedef struct efi_header {
    uint8_t header[8];
    uint32_t revision;
    uint32_t header_size;
    uint32_t check_sum;
    uint32_t reserved;
    uint64_t lba_address;
    uint64_t lba_alternative;
    uint64_t first_usable_block;
    uint64_t last_usable_block;
    uint8_t guid[16];
    uint64_t partition_entry_array_start;
    uint32_t entry_num;
    uint32_t entry_size;
    uint32_t crc32;
} efi_header_t;

#endif
