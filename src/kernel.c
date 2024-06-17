#include "mod/builtin_shell/stdio.h"
#include "mod/kheap/kheap_utils.h"
#include "drivers/ata/pata.h"
#include "drivers/partition/gpt.h"
#include "mod/algorithms/rng.h"
#include "mod/algorithms/guid.h"

#include <stdint.h>

void kernel_main() {
    kheap_init();

    kprintf("\n----ATA drive detection----\n");

    ata_drive_init();
    kprintf("\n");

    // create_gpt(&ata_primary_drive);
    guid_t temp;
    new_guid(&temp);
    uint16_t name[36] = {1};
    kprintf("%u ", create_partition(&ata_primary_drive, &temp, 100, 100, name));
    kprintf("%u ", create_partition(&ata_primary_drive, &temp, 200, 100, name));

    current_io_state = stdin_command;
    kprintf("root > ");
    
    for(;;) {
        asm("hlt");
    }
}
