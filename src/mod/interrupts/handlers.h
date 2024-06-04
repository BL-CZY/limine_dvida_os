#include <stdint.h>

#include "../../lib/std/stdio.h"
#include "../../lib/time/time.h"
#include "../../lib/std/keyboard.h"
#include "../../lib/utils/pic_utils.h"
#include "../../lib/utils/asm_utils.h"
#include "../../lib/utils/handle_utils.h"

typedef struct interrupt_info interrupt_info_t;

void page_fault(interrupt_info_t *info);

void irq_0();
void irq_1();

void isr_handler(interrupt_info_t *info);