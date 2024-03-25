#ifndef PIC_H
#define PIC_H

#include <stdint.h>
#include "../../lib/utils/asm_utils.h"

void pic_init();
void pic_config(uint8_t offset_pic1, uint8_t offset_pic2);

#endif