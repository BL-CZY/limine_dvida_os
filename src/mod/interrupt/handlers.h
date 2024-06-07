#include <stdint.h>

#include "mod/builtin_shell/stdio.h"
#include "mod/other_utils/time.h"
#include "mod/other_utils/pic_utils.h"
#include "mod/other_utils/asm_utils.h"
#include "mod/other_utils/handle_utils.h"

#include "drivers/keyboard/keyboard.h"

typedef struct interrupt_info interrupt_info_t;

void page_fault(interrupt_info_t *info);

void irq_0();
void irq_1();

void isr_handler(interrupt_info_t *info);