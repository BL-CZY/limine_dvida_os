#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <limine.h>

#include "lib/utils/mem_utils.h"
#include "lib/utils/asm_utils.h"
#include "lib/std/stdio.h"
#include "lib/memory/pmm.h"

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

static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};
 
// Halt and catch fire function.
static void hcf(void) {
    asm ("cli");
    for (;;) {
        asm ("hlt");
    }
}

extern void gdt_init(void);
extern void idt_init(void);
extern void pic_init();
extern void kernel_main();

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

    gdt_init();
    idt_init();
    enable_interrupt();
    pic_init();
 
    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    // initialize the terminal
    terminal_init(framebuffer->address, framebuffer->width, framebuffer->height, 3);
    terminal_set_resolution(860, 512);
    current_io_state = stdout;
    /**
     * do whatever for the setup texts
    */
    pmm_init(*memmap_request.response, *hhdm_request.response);
    current_io_state = stdin_command;
    printf("root > ");

    kernel_main();
 
    // We're done, just hang...
    hcf();
}