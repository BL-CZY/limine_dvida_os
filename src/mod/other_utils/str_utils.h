//implemented in mod/utils/str_utils.c

#ifndef STR_UTILS_H
#define STR_UTILS_H

#include <stddef.h>
#include <stdbool.h>

bool strcmp(char* first, char* second);
bool strcmp_withlen(char *first, char* second, size_t len);
size_t strlen(char *str);

#endif