#include "handlers.h"
#include "../../lib/std/stdio.h"

typedef struct interrupt_info
{
    uint64_t pushed_rsp;
    uint64_t cr4, cr3, cr2, cr0;
    uint64_t gs, fs, es, ds;
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rsp, rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t int_num, err;
    uint64_t rip, cs, eflags, rsp_sys, ss;
} interrupt_info_t;

void __attribute__((cdecl)) isr_handler(interrupt_info_t *info) {
    printf("%u", info->int_num);
    for(;;){
        asm("hlt");
    }
}