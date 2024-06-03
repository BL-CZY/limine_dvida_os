#include <limine.h>

#include "lib/std/stdio.h"
#include "lib/utils/mem_utils.h"
#include "lib/utils/general_utils.h"

#include "mod/drives/pata.h"

void kernel_main() {
    kheap_init();

    current_io_state = stdin_command;
    kprintf("root > ");

    for(int i = 0; i < 512; ++i) {
        sector_buffer[i] = 3;
    }

    kprintf("%u ", write_sector(0));
    kprintf("%u ", write_sector(0));
    
    for(;;) {
        asm("hlt");
    }
}
