#include "./str_utils.h"
#include "mod/builtin_shell/stdio.h"

bool strcmp_withlen(char *first, char* second, size_t len) {
    for(size_t i = 0; i < len; ++i)
    {
        if(first[i] != second[i])
        {
            return false;
        }
    }

    return true;
}

bool strcmp(char* first, char* second)
{
    if(strlen(first) != strlen(second)) {
        return false;
    }

    size_t length = strlen(first);

    for(size_t i = 0; i < length; ++i)
    {
        if(first[i] != second[i])
        {
            return false;
        }
    }

    return true;
}

size_t strlen(char *str) {
    size_t result = 0;
    while(str[result]) {
        ++result;
    }
    return result;
}