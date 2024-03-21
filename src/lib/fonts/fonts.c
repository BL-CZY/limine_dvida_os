#include <stdint.h>
#include "fonts.h"

typedef struct font_char
{
    uint16_t height;
    uint16_t width;
    uint16_t character[16];
} font_char_t;