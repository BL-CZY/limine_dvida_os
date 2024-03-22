#ifndef TERMINAL_H
#define TERMINAL_H

#include <stddef.h>
#include <stdint.h>

extern uint32_t *terminal_buffer_addr;
extern uint64_t terminal_width;
extern uint64_t terminal_height;
extern size_t current_row;
extern size_t current_col;
extern size_t cursor_row;
extern size_t cursor_col;
extern uint32_t bg_color;
extern uint32_t fg_color;

size_t strlen(char* str);
void set_cursor(int row, int col, int remove);
void terminal_putchar(char character);
void terminal_advance();

#endif