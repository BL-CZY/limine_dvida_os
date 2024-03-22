#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <limine.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>

#include "lib/utils/mem_utils.h"
#include "lib/std/stdio.h"
#include "mod/terminal/terminal.h"

// Set the base revision to 1, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.
 
static volatile LIMINE_BASE_REVISION(1);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once.
 
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};
 
// Halt and catch fire function.
static void hcf(void) {
    asm ("cli");
    for (;;) {
        asm ("hlt");
    }
}

// The following will be our kernel's entry point.
// If renaming _start() to something else, make sure to change the
// linker script accordingly.
void _start(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }
 
    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL
     || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }
 
    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    for(int i = 0; i < 860; ++i) {
        uint32_t *ptr = framebuffer->address;
        ptr[i] = 0xFFFFFF;
    }

    terminal_init(framebuffer->address, framebuffer->width, framebuffer->height);
    printf("Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!WWW");

    asm ("cli");
    for (;;) {
        asm ("hlt");
    }
 
    // We're done, just hang...
    hcf();
}