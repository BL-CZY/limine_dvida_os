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
    new_vector(sizeof(uint32_t), &test);
    for(uint32_t i = 0; i < 10; ++i) {
        test.push(&test, &i);
    }

    test.pop(&test);
    uint32_t temp_val = 30;
    test.insert(&test, &temp_val, 3);
    test.remove(&test, 2);

    for(uint32_t i = 0; i < test.count; ++i) {
        uint32_t temp;
        test.get(&test, i, &temp);
        kprintf("%u ", temp);
    }

    current_io_state = stdin_command;
    kprintf("root > ");
    
    for(;;) {
        asm("hlt");
    }
}
