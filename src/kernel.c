#include <limine.h>

#include "lib/std/stdio.h"
#include "lib/utils/mem_utils.h"
#include "lib/utils/general_utils.h"
#include "lib/utils/ata_utils.h"

void kernel_main() {
    kheap_init();

    kprintf("\n----ATA drive detection----\n");

    ata_drive_init();
    kprintf("\n");

    current_io_state = stdin_command;
    kprintf("root > ");
    
    for(;;) {
        asm("hlt");
    }
}
