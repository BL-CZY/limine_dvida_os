[bits 64]

global outb
global inb
global outw
global inw
global io_wait

global halt_cpu
global enable_interrupt
global disable_interrupt

section .text

outb:
    mov rdx, rdi
    mov rax, rsi
    out dx, al
    ret

inb:
    mov rdx, rdi
    in al, dx
    ret

outw:
    mov rdx, rdi
    mov rax, rsi
    out dx, ax
    ret

inw:
    mov rdx, rdi
    in ax, dx
    ret

io_wait:
    mov rdi, 0x80
    mov rsi, 0
    call outb
    ret

halt_cpu:
    hlt
    ret

enable_interrupt:
    sti
    ret

disable_interrupt:
    cli
    ret