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
    uint32_t test_val = 0x1;
    vector_push(&test_val, &test);
    test_val = 0x2;
    vector_push(&test_val, &test);
    test_val = 0x3;
    vector_push(&test_val, &test);
    test_val = 0x4;
    vector_push(&test_val, &test);
    test_val = 0x5;
    vector_push(&test_val, &test);
    test_val = 0x6;
    vector_push(&test_val, &test);
    test_val = 0x7;
    vector_push(&test_val, &test);
    test_val = 0x8;
    vector_push(&test_val, &test);
    test_val = 0x9;
    vector_push(&test_val, &test);
    test_val = 0x21938923;
    vector_insert(&test, &test_val, 3);
    vector_remove(&test, 2);
    for(uint32_t i = 0; i < vector_size(&test); ++i) {
        uint32_t test_res;
        vector_get(&test, i, &test_res);
        kprintf("%x ", test_res);
    }
    
    current_io_state = stdin_command;
    kprintf("root > ");
    
    for(;;) {
        asm("hlt");
    }
}
