#include <stdint.h>

typedef struct interrupt_info interrupt_info_t;

void __attribute__((cdecl)) isr_handler(interrupt_info_t *info);