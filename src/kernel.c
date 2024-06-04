#include <limine.h>

#include "lib/std/stdio.h"
#include "lib/utils/mem_utils.h"
#include "lib/utils/general_utils.h"

#include "mod/drives/pata.h"

void kernel_main() {
    kheap_init();

    current_io_state = stdin_command;
    kprintf("root > ");

    if(identify_ata_drive(ATA_DRIVE_PORT) != 0) {
        
    }

    for(int i = 0; i < 512; ++i) {
        sector_buffer[i] = 10;
    }

    pio_read_sector(0x80);

    for(int i = 0; i < 512; ++i) {
        kprintf("%u ", sector_buffer[i]);
    }
    
    for(;;) {
        asm("hlt");
    }
}
