#ifndef GPT_H
#define GPT_H

#include "hal/storage/hal_storage.h"
#include "mod/algorithms/guid.h"
#include "mod/algorithms/crc32.h"
#include "mod/mmu/mem_utils.h"

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

//this function checks if the gpt table is here
bool is_gpt_present(uint8_t *buffer);
bool is_entry_unused(uint8_t *buffer);

//this function creates a new gpt table
void create_gpt(storage_device_t *drive);

void read_entry(uint8_t *buffer, gpt_table_entry_t *result, uint16_t *entry_count);
void write_entry_to_buffer(gpt_table_entry_t *input, uint8_t *result);

#define READ_GPT_ERR_NO_GPT 1
#define READ_GPT_ERR_CRC32_DOESNT_MATCH_FOR_HEADER 2
#define READ_GPT_ERR_CRC32_DOESNT_MATCH_FOR_ARRAY 3

//this function reads the gpt table
int read_gpt(storage_device_t *drive, gpt_efi_header_t *result_header, gpt_table_t *result_table);

void overwrite_gpt(storage_device_t *drive, gpt_efi_header_t *header, gpt_table_t *table);

int create_partition(storage_device_t *drive, guid_t *type_guid, uint64_t start_lba, uint64_t length, uint16_t *name);

int delete_partition(storage_device_t *drive, size_t index);

#endif
