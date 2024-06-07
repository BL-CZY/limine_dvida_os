#include <stdint.h>

#include "./gpt.h"

#include "drivers/ata/pata.h"
#include "mod/other_utils/general_utils.h"

const uint8_t GPT_EFI_HEADER[8] = {0x45, 0x46, 0x49, 0x20, 0x50, 0x41, 0x52, 0x54};
const uint8_t EFI_REVISION[4] = {0x00, 0x00, 0x01, 0x00};

bool is_gpt_present(ata_drive_t *drive) {
    //TODO use DMA
    uint8_t buffer[512];
    pio_read_sector(drive, 1, 1, false, buffer);
    for(int i = 0; i < 8; ++i) {
        if(buffer[i] != GPT_EFI_HEADER[i]) {
            return false;
        }
    }
    return true;
}

void create_gpt(ata_drive_t *drive) {
    uint64_t alt_lba;
    if(drive->is_lba48_supported) {
        alt_lba = drive->lba48_sector_count - 2;
    } else {
        alt_lba = drive->lba28_sector_count - 2;
    }

    gpt_efi_header_t result = {
        .header = GPT_EFI_HEADER,
        .revision = EFI_REVISION,
        .header_size = 92,
        .reserved = 0,
        .header_lba_address = 1,
        .backup_header_lba_address = alt_lba,
        .partition_array_start_lba = 1,
        .entry_num = 128,
        .entry_size = 128,
    };
}

//* error codes:
/**
    0: no error
    1: gpt not present    
*/
int read_gpt(ata_drive_t *drive, gpt_efi_header_t *result_header, gpt_table_t *result_table) {
    return 0;
}