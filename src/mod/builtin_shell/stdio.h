#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

typedef enum io_state
{
    stdin_command,
    stdin_data,
    stdout,
} io_state_t;
extern io_state_t current_io_state;
void terminal_init(void *addr, const uint64_t width, const uint64_t height, const int blink_speed);
void terminal_set_resolution(const uint64_t width, const uint64_t height);
void kprintf(char *format, ...);
void erasef(size_t num);
void terminal_clear();

#endif