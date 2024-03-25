#include <stdint.h>

typedef struct interrupt_info interrupt_info_t;

void irq_0();

void isr_handler(interrupt_info_t *info);