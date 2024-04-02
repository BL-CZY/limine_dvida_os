#include <limine.h>

#include "lib/std/stdio.h"
#include "lib/utils/mem_utils.h"
#include "mod/file_system/fs_driver.h"

void kernel_main() {
    kheap_init();
    // read_sector(0);

    // for(int i = 0; i < 512; ++i) {
    //     printf("%u", sector_buffer[i]);
    // }

    current_io_state = stdin_command;
    printf("root > ");
    for(;;) {
        asm("hlt");
    }
}
