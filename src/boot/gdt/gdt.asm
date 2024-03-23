[bits 64]

section .data

gdt_start:
    .null_descriptor:
        dq 0
    .kernel_code_descriptor:
        dw 0
        dw 0
        db 0
        db 0b10011010
        db 0b10100000
        db 0
    .kernel_data_descriptor:
        dw 0
        dw 0
        db 0
        db 0b10010010
        db 0b10100000
        db 0
    .user_code_descriptor:
        dw 0
        dw 0
        db 0
        db 0b11111010
        db 0b10100000
        db 0
    .user_data_descriptor:
        dw 0
        dw 0
        db 0
        db 0b11110010
        db 0b10100000
        db 0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dq gdt_start

global gdt_init
section .text
    
gdt_init:
    lgdt[gdt_descriptor]
    push 0x08 ; code segment
    lea rax, [rel .reload_cs]
    push rax ; relative address
    retfq ; far return

.reload_cs:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret