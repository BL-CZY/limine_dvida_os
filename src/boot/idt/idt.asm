[bits 64]

section .data

idt_start:
    times 128 * 256 db 0
idt_end:

idt_descriptor:
    .limit: dw idt_end - idt_start - 1
    .base: dq idt_start

section .text

global idt_init
extern isr_stub_table

idt_init:
    mov rax, 0 ; initialize the counter

    .set_entry:
        cmp rax, 256 ; if counter is 256 then jump to end
        je .end ; jump if equal

        mov rbx, [isr_stub_table + rax * 8] ; mov the address of the handler to rbx
        mov rcx, rax
        imul rcx, 16

        mov word [idt_start + rcx], bx ; get the lower 16 bits of the address
        mov word [idt_start + rcx + 2], 0x08 ; set the kernel code segment selector
        mov byte [idt_start + rcx + 4], 0x00 ; reserved
        mov byte [idt_start + rcx + 5], 0b10001111 ; set flags

        shr rbx, 16 ; get the higher 16 bits of the address
        mov word [idt_start + rcx + 6], bx ; set the address of the handler
        shr rbx, 16
        mov dword [idt_start + rcx + 8], ebx
        mov dword [idt_start + rcx + 12], 0

        inc rax ; increment the counter
        jmp .set_entry ; loop

    .end:
        lidt[idt_descriptor]

    ret