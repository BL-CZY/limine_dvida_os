#include "gpt.h"
#include "../../lib/file_system/partition.h"

const uint8_t GPT_EFI_HEADER[8] = {0x45, 0x46, 0x49, 0x20, 0x50, 0x41, 0x52, 0x54};

bool is_gpt_present() {
    pio_read_sector(1);

    //check for the header
    for(int i = 0; i < 8; ++i) {
        if(sector_buffer[i] != GPT_EFI_HEADER[i]) {
            return false;
        }
    }
    
    return true;
}

void create_gpt() {

}

gpt_table_t *read_gpt () {
    pio_read_sector(1);

    

    return 0;
}