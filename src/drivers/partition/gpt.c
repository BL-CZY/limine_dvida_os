#include <stdint.h>

#include "./gpt.h"

#include "drivers/ata/pata.h"
#include "mod/other_utils/general_utils.h"
#include "mod/algorithms/crc32.h"

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

void create_gpt(ata_drive_t *drive) {
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

    // set the guid as the serial number of the disk
    for(int i = 0; i < 4; ++i) {
        result.disk_guid.data1[i] = drive->serial[i]; // big endian
    }

    for(int i = 0; i < 2; ++i) {
        result.disk_guid.data2[i] = drive->serial[i + 4]; // big endian
    }

    for(int i = 0; i < 2; ++i) {
        result.disk_guid.data3[i] = drive->serial[i + 6]; // big endian
    }

    for(int i = 0; i < 2; ++i) {
        result.disk_guid.data4[i] = drive->serial[8 + i]; // big endian
    }

    for(int i = 0; i < 6; ++i) {
        result.disk_guid.data5[i] = drive->serial[10 + i]; // big endian
    }

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
    // TODO use DMA

    // primary
    pio_write_sector(drive, 1, 1, efi_header);
    pio_write_sector(drive, 2, 32, partition_array);

    // secondary
    pio_write_sector(drive, -1, 1, efi_header);
    pio_write_sector(drive, -33, 32, partition_array);

    #pragma endregion
}

void read_entry(uint8_t *buffer, gpt_table_entry_t *result) {
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

//* error codes:
/**
    0: no error
    1: gpt not present    
    2: crc32 doesn't match for header
    3: crc32 doesn't match for array
*/
int read_gpt(ata_drive_t *drive, gpt_efi_header_t *result_header, gpt_table_t *result_table) {
    uint8_t efi_header_buffer[512];
    pio_read_sector(drive, 1, 1, efi_header_buffer);

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
    pio_read_sector(drive, 2, result_header->entry_size * result_header->entry_num / 512, array_buffer);

    // verify the crc32
    if(!(full_crc(array_buffer, result_header->entry_size * result_header->entry_num) == result_header->array_crc32)) {
        // crc32 doesn't match
        return 3;
    }

    // read through the buffer
    for(uint16_t i = 0; i < result_header->entry_num; ++i) {
        read_entry(array_buffer + (i * result_header->entry_size), &result_table->entries[i]);
    }

    #pragma endregion

    return 0;
}

int create_partition(ata_drive_t *drive, guid_t *type_guid) {}