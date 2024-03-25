#include <limine.h>

#include "lib/std/stdio.h"

void kernel_main() {
    for(;;) {
        asm("hlt");
    }
}