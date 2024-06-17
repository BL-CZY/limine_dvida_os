#include "./handlers.h"

typedef struct interrupt_info
{
    uint64_t cr4, cr3, cr2, cr0;
    uint64_t gs, fs, es, ds;
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rsp, rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t int_num, err;
    uint64_t rip, cs, eflags, rsp_sys, ss;
} interrupt_info_t;

void page_fault(interrupt_info_t *info) {
    kprintf("page fault: %b\n", info->err);
    while(1)
    {
        asm("hlt");
    }
    
}

void irq_0() {
    on_update();
}

void irq_1() {
    process_key(inb(0x60));
}

void irq_14() {}

void irq_15() {}

void irq_handler(interrupt_info_t *info) {
    switch(info->int_num - 32)
    {        
        case 0:
            irq_0();
            break;
        case 1:
            irq_1();
            break;
        case 14:
            irq_14();
            break;
        case 15:
            irq_15();
            break;
        default:
            kprintf("unhandled irq number %u\n", info->int_num - 32);
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
        case 14:
            page_fault(info);
            break;
        default:
            kprintf("unhandled isr number %u\n", info->int_num);
            break;
    }
}

void kpanic(char *msg) {
    kprintf("%ce----------KERNEL PANIC!!!----------%cd\n");
    kprintf(msg);
    disable_interrupt();
    for(;;){
        halt_cpu();
    }
}
