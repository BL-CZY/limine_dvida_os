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
    kprintf("\n");

    vector_t test;
    new_vector(4, &test);
    uint32_t test_val = 0xABCDEF99;
    vector_push(&test_val, &test);
    uint32_t test_res;
    vector_get(&test, 0, &test_res);
    kprintf("%x ", test_res);
    test_val = 0xFFFFFFFF;
    vector_set(&test, 0, &test_val);
    vector_get(&test, 0, &test_res);
    kprintf("%x ", test_res);
    
    current_io_state = stdin_command;
    kprintf("root > ");
    
    for(;;) {
        asm("hlt");
    }
}
