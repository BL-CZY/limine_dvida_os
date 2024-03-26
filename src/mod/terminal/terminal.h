#ifndef TERMINAL_H
#define TERMINAL_H

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

extern uint32_t *terminal_buffer_addr;
extern uint64_t terminal_width;
extern uint64_t terminal_height;
extern size_t current_row;
extern size_t current_col;
extern size_t cursor_row;
extern size_t cursor_col;
extern uint32_t bg_color;
extern uint32_t fg_color;
extern int cursor_blink_phase;
extern int cursor_blink_speed;

size_t strlen(char* str);
void update_cursor_blink();
void set_cursor(int row, int col, int remove);
void render_char(char character, int row, int col);
void terminal_putchar(char character);
void terminal_advance();
void terminal_back();
void printf_no_color(char* format);

#endif