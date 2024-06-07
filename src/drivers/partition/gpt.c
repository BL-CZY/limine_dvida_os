#include <stdint.h>

#include "./gpt.h"
#include "./partition.h"

const uint8_t GPT_EFI_HEADER[8] = {0x45, 0x46, 0x49, 0x20, 0x50, 0x41, 0x52, 0x54};

bool is_gpt_present(ata_drive_t *drive) {
    return true;
}

void create_gpt(ata_drive_t *drive) {

}

//* error codes:
/**
    0: no error
    1: gpt not present    
*/
int read_gpt(ata_drive_t *drive, gpt_efi_header_t *result_header, gpt_table_t *result_table) {
    return 0;
}