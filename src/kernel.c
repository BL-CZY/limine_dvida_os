#include "lib/std/stdio.h"
#include "lib/utils/mem_utils.h"
#include "lib/utils/ata_utils.h"

#include <stdint.h>

void kernel_main() {
    kheap_init();

    kprintf("\n----ATA drive detection----\n");

    ata_drive_init();
    kprintf("\n");

    uint8_t buffer[1024];

    for(int i = 0; i < 1024; ++i) {
        buffer[i] = 0;
    }
    kprintf("%u", pio_write_sector(&ata_primary_drive, 0, 2, false, buffer));

    current_io_state = stdin_command;
    kprintf("root > ");
    
    for(;;) {
        asm("hlt");
    }
}
