#include <stdint.h>

#include "./gpt.h"
#include "mod/builtin_shell/stdio.h"

static const uint8_t GPT_EFI_SIGNATURE[8] = {0x45, 0x46, 0x49, 0x20, 0x50, 0x41, 0x52, 0x54};
static const uint8_t EFI_REVISION[4] = {0x00, 0x00, 0x01, 0x00};

bool is_gpt_present(uint8_t *buffer) {
    for(int i = 0; i < 8; ++i) {
        if(buffer[i] != GPT_EFI_SIGNATURE[i]) {
            return false;
        }
    }
    return true;
}

bool is_entry_unused(uint8_t *buffer) {
    for(int i = 0; i < 16; ++i) {
        if(buffer[i] != 0) {
            return false;
        }
    }

    return true;
}

void create_gpt(storage_device_t *drive) {
    #pragma region header_struct

    uint64_t alt_lba;
    if(drive->is_lba48_supported) {
        alt_lba = drive->lba48_sector_count - 1;
    } else {
        alt_lba = drive->lba28_sector_count - 1;
    }

    // set some basic information
    gpt_efi_header_t result = {
        .header_size = 92,
        .reserved = 0,
        .header_lba_address = 1,
        .backup_header_lba_address = alt_lba,
        .first_usable_block_lba = 34,
        .last_usable_block_lba = alt_lba - 33, // -34 lba
        .partition_array_start_lba = 2,
        .entry_num = 128,
        .entry_size = 128,
    };

    // set signature and revision

    for(int i = 0; i < 8; ++i) {
        result.signature[i] = GPT_EFI_SIGNATURE[i];
    }

    for(int i = 0; i < 4; ++i) {
        result.signature[i] = EFI_REVISION[i];
    }

    // create guid
    new_guid(&result.disk_guid);

    // initialize an empty array
    uint8_t partition_array[32 * 512];

    for(int i = 0; i < 32 * 512; ++i) {
        partition_array[i] = 0;
    }

    result.array_crc32 = full_crc(partition_array, 32 * 512);

    #pragma endregion

    #pragma region to_buffer

    // convert the header into a buffer
    uint8_t efi_header[512];

    // signature
    for(int i = 0; i < 8; ++i) {
        efi_header[i] = GPT_EFI_SIGNATURE[i];
    }

    // revision
    for(int i = 0; i < 4; ++i) {
        efi_header[8 + i] = EFI_REVISION[i];
    }

    // header size
    uint32_to_little_endian(result.header_size, (efi_header + 12));

    // crc32 & reserved
    for(int i = 0; i < 8; ++i) {
        efi_header[16 + i] = 0;
    }

    // current LBA
    uint64_to_little_endian(result.header_lba_address, (efi_header + 24));

    // alternative LBA
    uint64_to_little_endian(result.backup_header_lba_address, (efi_header + 32));

    // first usable lba
    uint64_to_little_endian(result.first_usable_block_lba, (efi_header + 40));

    // last usable lba
    uint64_to_little_endian(result.last_usable_block_lba, (efi_header + 48));

    // guid
    for(int i = 0; i < 4; ++i) {
        efi_header[56 + i] = result.disk_guid.data1[3 - i]; // little endian
    }

    for(int i = 0; i < 2; ++i) {
        efi_header[60 + i] = result.disk_guid.data2[1 - i]; // little endian
    }

    for(int i = 0; i < 2; ++i) {
        efi_header[62 + i] = result.disk_guid.data3[1 - i]; // little endian
    }

    for(int i = 0; i < 2; ++i) {
        efi_header[64 + i] = result.disk_guid.data4[i]; // big endian
    }

    for(int i = 0; i < 6; ++i) {
        efi_header[66 + i] = result.disk_guid.data5[i]; // big endian
    }

    // startig lba
    uint64_to_little_endian(result.partition_array_start_lba, (efi_header + 72));

    // number of partition entries
    uint32_to_little_endian(result.entry_num, (efi_header + 80));

    // partition entry size
    uint32_to_little_endian(result.entry_size, (efi_header + 84));

    // array crc32
    uint32_to_little_endian(result.array_crc32, (efi_header + 88));

    // now calculate the crc32 of the header
    result.header_crc32 = full_crc(efi_header, 512);

    // header crc32
    uint32_to_little_endian(result.header_crc32, (efi_header + 16));

    // write to the disk

    // primary
    write_sectors(drive, 1, 1, efi_header);
    write_sectors(drive, 2, 32, partition_array);

    // secondary
    write_sectors(drive, -1, 1, efi_header);
    write_sectors(drive, -33, 32, partition_array);

    #pragma endregion
}

void read_entry(uint8_t *buffer, gpt_table_entry_t *result, uint16_t *entry_count) {
    if(!is_entry_unused(buffer)) {
        ++*entry_count;
    }

    // read the partition type guid
    buffer_to_guid(buffer, &result->partition_type_guid);

    // read the unique guid
    buffer_to_guid(buffer + 16, &result->unique_partition_guid);

    // start and end lba, both inclusive
    result->start_lba = little_endian_to_uint64(buffer + 32);
    result->end_lba = little_endian_to_uint64(buffer + 40);

    // flags
    result->flags = little_endian_to_uint64(buffer + 48);

    // name
    for(int i = 0; i < 36; ++i) {
        result->utf16_name[i] = little_endian_to_uint16(buffer + 56 + (i * 2));
    }
}

void write_entry_to_buffer(gpt_table_entry_t *input, uint8_t *result) {
    // guid
    guid_to_buffer(&input->partition_type_guid, result);
    guid_to_buffer(&input->unique_partition_guid, result + 16);

    // start and end lba
    uint64_to_little_endian(input->start_lba, result + 32);
    uint64_to_little_endian(input->end_lba, result + 40);

    // flags
    uint64_to_little_endian(input->flags, result + 48);

    // name
    for(int i = 0; i < 36; ++i) {
        uint8_t buffer[2];
        uint16_to_little_endian(input->utf16_name[i], buffer);
        result[56 + (i * 2)] = buffer[0];
        result[56 + (i * 2) + 1] = buffer[1];
    }
}

//* error codes:
/**
    0: no error
    1: gpt not present    
    2: crc32 doesn't match for header
    3: crc32 doesn't match for array
*/
int read_gpt(storage_device_t *drive, gpt_efi_header_t *result_header, gpt_table_t *result_table) {
    uint8_t efi_header_buffer[512];
    read_sectors(drive, 1, 1, efi_header_buffer);

    if(!is_gpt_present(efi_header_buffer)) {
        // gpt not present
        return 1;
    }

    // get the crc32 check sum
    uint32_t header_crc32 = little_endian_to_uint32(efi_header_buffer + 16);

    // clear the crc32
    for(int i = 16; i < 20; ++i) {
        efi_header_buffer[i] = 0;
    }

    // verify crc32
    if(header_crc32 != full_crc(efi_header_buffer, 512)) {
        return 2;
    }

    #pragma region set_header

    // set signature
    for(int i = 0; i < 8; ++i) {
        result_header->signature[i] = efi_header_buffer[i];
    }

    // set revision
    for(int i = 0; i < 4; ++i) {
        result_header->revision[i] = efi_header_buffer[8 + i];
    }

    // header size
    result_header->header_size = little_endian_to_uint32(efi_header_buffer + 12);

    // crc32
    result_header->header_crc32 = header_crc32;

    // reserved is 0
    result_header->reserved = 0;

    // header lba
    result_header->header_lba_address = little_endian_to_uint64(efi_header_buffer + 24);

    // backup lba
    result_header->backup_header_lba_address = little_endian_to_uint64(efi_header_buffer + 32);

    // first usable lba
    result_header->first_usable_block_lba = little_endian_to_uint64(efi_header_buffer + 40);

    // last usable lba
    result_header->last_usable_block_lba = little_endian_to_uint64(efi_header_buffer + 48);

    // guid
    buffer_to_guid(efi_header_buffer + 56, &result_header->disk_guid);

    // starting lba of array
    result_header->partition_array_start_lba = little_endian_to_uint64(efi_header_buffer + 72);

    // no. of partitions
    result_header->entry_num = little_endian_to_uint32(efi_header_buffer + 80);

    // entry size
    result_header->entry_size = little_endian_to_uint32(efi_header_buffer + 84);

    // array crc32
    result_header->array_crc32 = little_endian_to_uint32(efi_header_buffer + 88);

    #pragma endregion

    #pragma region array

    // read the array
    uint8_t array_buffer[result_header->entry_size * result_header->entry_num];
    read_sectors(drive, result_header->partition_array_start_lba, result_header->entry_size * result_header->entry_num / 512, array_buffer);

    // verify the crc32
    if(!(full_crc(array_buffer, result_header->entry_size * result_header->entry_num) == result_header->array_crc32)) {
        // crc32 doesn't match
        return 3;
    }

    // read through the buffer
    for(uint16_t i = 0; i < result_header->entry_num; ++i) {
        read_entry(array_buffer + (i * result_header->entry_size), &result_table->entries[i], &result_table->entry_count);
    }

    #pragma endregion

    return 0;
}

void overwrite_gpt(storage_device_t *drive, gpt_efi_header_t *header, gpt_table_t *table) {
    // compute the array
    uint8_t array_buffer[header->entry_num * header->entry_size];

    for(uint32_t i = 0; i < header->entry_num; ++i) {
        write_entry_to_buffer(&table->entries[i], array_buffer + (i * header->entry_size));
    }

    // calculate the array crc32
    header->array_crc32 = full_crc(array_buffer, header->entry_num * header->entry_size);

    // zero out the header crc32
    header->header_crc32 = 0;

    // convert the header into a buffer
    uint8_t header_buffer[512];

    // copy the signature
    memcpy((void *)header_buffer, (void *)header->signature, 8);

    // copy the revision num
    memcpy((void *)(header_buffer + 8), (void *)header->revision, 4);

    // header size
    uint32_to_little_endian(header->header_size, header_buffer + 12);

    // crc32 and reserved
    memset(header_buffer + 16, 0, 8);

    // current lba
    uint64_to_little_endian(header->header_lba_address, header_buffer + 24);

    // alternative lba
    uint64_to_little_endian(header->backup_header_lba_address, header_buffer + 32);

    // first usable lba
    uint64_to_little_endian(header->first_usable_block_lba, header_buffer + 40);

    // last usable lba
    uint64_to_little_endian(header->last_usable_block_lba, header_buffer + 48);

    // guid
    guid_to_buffer(&header->disk_guid, header_buffer + 56);

    // partition entry start lba
    uint64_to_little_endian(header->partition_array_start_lba, header_buffer + 72);

    // num of entries
    uint32_to_little_endian(header->entry_num, header_buffer + 80);

    // entry size
    uint32_to_little_endian(header->entry_size, header_buffer + 84);

    // array crc32
    uint32_to_little_endian(header->array_crc32, header_buffer + 88);

    // all empty
    memset(header_buffer + 92, 0, 420);

    // calculate the crc32
    header->header_crc32 = full_crc(header_buffer, 512);
    uint32_to_little_endian(header->header_crc32, header_buffer + 16);

    // write to the disk
    // primary
    write_sectors(drive, 1, 1, header_buffer);
    write_sectors(drive, 2, 32, array_buffer);

    // secondary
    write_sectors(drive, -1, 1, header_buffer);
    write_sectors(drive, -33, 32, array_buffer);
}

// error codes:
// 1: GPT not present
// 2: crc32 doesn't match for header
// 3: crc32 doesn't match for awway
// 4: no more entries
int create_partition(storage_device_t *drive, guid_t *type_guid, uint64_t start_lba, uint64_t length, uint16_t *name) {
    gpt_efi_header_t header;
    gpt_table_t table;

    int error_code = read_gpt(drive, &header, &table);

    if(error_code != 0) {
        return error_code;
    }

    if(!is_gpt_present(header.signature)) {
        return 1;
    }

    if(table.entry_count >= 128) {
        return 4;
    }

    gpt_table_entry_t *target = NULL;

    // iterate through the table to find an unused one
    for(int i = 0; i < 32; ++i) {
        uint8_t buffer[16];
        guid_to_buffer(&table.entries[i].partition_type_guid, buffer);
        if(is_entry_unused(buffer)) {
            target = &table.entries[i];
            break;
        }
    }

    if(target == NULL) {
        return 4;
    }

    // copy the partition type guid
    cpy_guid(type_guid, &target->partition_type_guid);

    // generate unique guid
    guid_t unique_guid;
    new_guid(&unique_guid);
    cpy_guid(&unique_guid, &target->unique_partition_guid);

    // first and last lba
    target->start_lba = start_lba;
    target->end_lba = start_lba + length - 1;

    // no flags when initialize
    target->flags = 0;

    // set name
    for(int i = 0; i < 36; ++i) {
        target->utf16_name[i] = name[i];
    }

    // increment the entry count
    ++table.entry_count;

    overwrite_gpt(drive, &header, &table);

    return 0;
}

// error code 4: index too large
int delete_partition(storage_device_t *drive, size_t index) {
    gpt_efi_header_t header;
    gpt_table_t table;

    int error_code = read_gpt(drive, &header, &table);

    if(error_code != 0) {
        return error_code;
    }

    if((uint32_t)index >= header.entry_num) {
        return 5;
    }

    if(!is_gpt_present(header.signature)) {
        return 1;
    }

    uint8_t empty_buffer[512];

    for(int i = 0; i < 512; ++i) {
        empty_buffer[i] = 0;
    }

    // erase all the data in the partition
    for(uint64_t i = table.entries[index].start_lba; i <= table.entries[index].end_lba; ++i) {
        write_sectors(drive, i, 1, empty_buffer);
    }

    // remove the partition from the table

    table.entry_count -= 1;
    guid_t empty_guid = {
        .data1 = {0, 0, 0, 0},
        .data2 = {0, 0},
        .data3 = {0, 0},
        .data4 = {0, 0},
        .data5 = {0, 0, 0, 0, 0},
    };

    cpy_guid(&empty_guid, &table.entries[index].partition_type_guid);
    cpy_guid(&empty_guid, &table.entries[index].unique_partition_guid);

    table.entries[index].start_lba = 0;
    table.entries[index].end_lba = 0;
    table.entries[index].flags = 0;
    
    for(int i = 0; i < 36; ++i) {
        table.entries[index].utf16_name[i] = 0;
    }

    overwrite_gpt(drive, &header, &table);

    return 0;
}