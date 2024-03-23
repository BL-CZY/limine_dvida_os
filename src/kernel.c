#include <limine.h>

#include "lib/std/stdio.h"

void kernel_main() {
    printf("%ce%s%cd%cc%u%cd\n", "test", 0x12345600000000,10);
}