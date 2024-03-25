#include "handlers.h"
#include "../../lib/std/stdio.h"
#include "../../lib/utils/pic_utils.h"
#include "../../lib/time/time.h"

typedef struct interrupt_info
{
    uint64_t cr4, cr3, cr2, cr0;
    uint64_t gs, fs, es, ds;
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rsp, rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t int_num, err;
    uint64_t rip, cs, eflags, rsp_sys, ss;
} interrupt_info_t;

void irq_0() {
    on_update();
}

void irq_handler(interrupt_info_t *info) {
    switch(info->int_num - 32)
    {        
        case 0:
            irq_0();
            break;
        default:
            printf("unhandled irq number %u\n", info->int_num - 32);
            break;
    }
    pic_send_end_of_interrupt(info->int_num - 32);
}

void isr_handler(interrupt_info_t *info) {
    if(info->int_num >= 32 && info->int_num < 48) {
        irq_handler(info);
        return;
    }
    switch(info->int_num)
    {
        default:
            printf("unhandled isr number %u\n", info->int_num);
            break;
    }
}