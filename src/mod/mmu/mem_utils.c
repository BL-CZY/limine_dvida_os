#include "./mem_utils.h"

void *memcpy(void *dest, const void *src, size_t length) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;
 
    for(size_t i = 0; i < length; i++) {
        pdest[i] = psrc[i];
    }
 
    return dest;
}
 
void *memset(void *dest, int value, size_t length) {
    uint8_t *p = (uint8_t *)dest;
 
    for(size_t i = 0; i < length; i++) {
        p[i] = (uint8_t)value;
    }
 
    return dest;
}
 
void *memmove(void *dest, const void *src, size_t length) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;
 
    if(src > dest) {
        for(size_t i = 0; i < length; i++) {
            pdest[i] = psrc[i];
        }
    } else if(src < dest) {
        for(size_t i = length; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }
 
    return dest;
}
 
int memcmp(const void *s1, const void *s2, size_t length) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;
 
    for(size_t i = 0; i < length; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }
 
    return 0;
}