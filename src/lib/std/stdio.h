#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include <stddef.h>

void terminal_init(void *addr, const uint64_t width, const uint64_t height);
void terminal_set_resolution(const uint64_t width, const uint64_t height);
void printf(char* format, ...);
void erasef(size_t num);

#endif