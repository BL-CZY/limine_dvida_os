#include <limine.h>

#include "lib/std/stdio.h"
#include "lib/utils/mem_utils.h"

void kernel_main() {
    kheap_init();

    current_io_state = stdin_command;
    kprintf("root > ");
    
    for(;;) {
        asm("hlt");
    }
}
