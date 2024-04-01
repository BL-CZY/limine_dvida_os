#include "kheap.h"
#include "../../lib/utils/mem_utils.h"
#include "../../lib/memory/pmm.h"

uint8_t *kheap_start;
uint8_t *kheap_end;
uint8_t *kheap_current_address;

void *malloc(size_t size) {
    /**
     * the block structure:
     * the heap is divided into 4 bytes blocks
     * every time it will allocate an extra block at the beginning to keep track of the information of this block
    */

    /**
     * allocations
     * reuse alloc: allocate from those freed
     * lazy alloc: allocate from the current location
     * brute force alloc: search the entire heap
    */

    //check for lazy alloc
    //TODO implement reuse alloc and brute force alloc
    size_t block_num = 1 + (size/4 + (size%4 > 0));
    int lazy_alloc = 1;

    if(kheap_current_address + size > kheap_end)
    {
        lazy_alloc = 0;
    }

    if(lazy_alloc)
    {
        //set the information about the first block
        *(uint32_t *)kheap_current_address = block_num * 4;
        void* result = kheap_current_address + 4;
        kheap_current_address += block_num * 4;
        return result;
    }

    //TODO placeholder, it will be a system interrupt call
    return 0;
}


void free(void *target) {
    void *start = target - 4;
    size_t size = (size_t)(*(uint32_t *)start);
    for(size_t i = 0; i < size; ++i)
    {
        *(char *)(start + i) = '\0';
    }
}

void* realloc(void *target, size_t new_size) {
    void *start = target;
    size_t size = *(size_t *)(start - 4);
    void *new_address = malloc(new_size);
    memcpy(start, new_address, size);
    free(start);
    return new_address;
}

void kheap_init() {
    //10 pages for kernel heap
    kheap_start = allocate_continuous_pages(10);
    kheap_end = kheap_start + 10 * PAGE_SIZE;
    kheap_current_address = kheap_start;
}