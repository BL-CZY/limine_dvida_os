#include "mod/builtin_shell/stdio.h"
#include "mod/kheap/kheap_utils.h"
#include "drivers/ata/pata.h"
#include "drivers/partition/gpt.h"
#include "mod/algorithms/rng.h"
#include "mod/algorithms/guid.h"
#include "mod/data_structs/vector.h"

#include <stdint.h>

void kernel_main() {
    kheap_init();

    kprintf("\n----ATA drive detection----\n");

    ata_drive_init();
    
    create_gpt(&ata_primary_drive);

    kprintf("\n");

    current_io_state = stdin_command;
    kprintf("root > ");
    
    for(;;) {
        asm("hlt");
    }
}
