//implemented in mod/utils/asm_utils.asm

#include <stdint.h>

extern void outb(uint16_t port, uint8_t value);
extern uint8_t inb(uint16_t port);
extern void outw(uint16_t port, uint8_t value);
extern uint16_t inw(uint16_t port);
extern void io_wait();

extern void halt_cpu();
extern void enable_interrupt();
extern void disable_interrupt();