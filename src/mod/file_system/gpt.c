#include "gpt.h"
#include "../../lib/file_system/partition.h"
#include <stdint.h>

const uint8_t GPT_EFI_HEADER[8] = {0x45, 0x46, 0x49, 0x20, 0x50, 0x41, 0x52, 0x54};

bool is_gpt_present() {    
    return true;
}

void create_gpt() {

}

gpt_table_t *read_gpt () {
    return 0;
}