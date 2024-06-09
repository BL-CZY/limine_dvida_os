#ifndef GPT_H
#define GPT_H

#include "drivers/ata/pata.h"

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

//* data 1, 2, and 3 are in little endian, and data 4 and 5 are in big endian
typedef struct guid {
    uint8_t data1[4];
    uint8_t data2[2];
    uint8_t data3[2];
    uint8_t data4[2];
    uint8_t data5[6];
} guid_t;

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

//this function creates a new gpt table
void create_gpt(ata_drive_t *drive);

//this function reads the gpt table
int read_gpt(ata_drive_t *drive, gpt_efi_header_t *result_header, gpt_table_t *result_table);

#endif
