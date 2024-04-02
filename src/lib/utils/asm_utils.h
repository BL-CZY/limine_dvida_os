//implemented in mod/utils/asm_utils.asm

#include <stdint.h>

extern void outb(uint16_t port, uint8_t value);
extern uint8_t inb(uint16_t port);
extern void outw(uint16_t port, uint16_t value);
extern uint16_t inw(uint16_t port);
extern void io_wait(void);

extern void halt_cpu(void);
extern void enable_interrupt(void);
extern void disable_interrupt(void);

extern uint64_t get_cr3(void);
extern void set_cr3(uint64_t val);