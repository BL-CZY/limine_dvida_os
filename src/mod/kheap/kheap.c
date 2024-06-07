#include "./kheap.h"
#include "mod/mmu/mem_utils.h"
#include "mod/kheap/kheap_utils.h"
#include "mod/other_utils/math_utils.h"
#include "mod/mmu/pmm.h"
#include "mod/builtin_shell/stdio.h"

uint8_t *kheap_start;
uint8_t *kheap_end;
uint8_t *kheap_current_address;

void *kmalloc(size_t size) {
    /**
     * the block structure:
     * the heap is divided into 4 bytes blocks
     * every time it will allocate an extra block at the beginning to keep track of the information of this block
    */

    if(size == 0) {
        return NULL;
    }

    size_t current_continuous_block = 0;
    uint8_t *start = kheap_start;

    for(size_t i = (size_t)kheap_start; i < (size_t)kheap_end; i += 4) {
        if(*(uint32_t *)i == 0) {
            ++current_continuous_block;
            if(current_continuous_block == 1) {
                start = (uint8_t *)i;
            } else if (current_continuous_block == round_up(size, 4) + 1) {
                break;
            }
        } else {
            current_continuous_block = 0;
        }
    }

    if(current_continuous_block != round_up(size, 4) + 1) {
        //TODO request more pages from the memory manager and retry
        return NULL;
    }

    *(uint32_t *)start = current_continuous_block;
    return (void *)(start + 4);
}


void kfree(void *target) {
    //null detection
    if(target == NULL) {
        return;
    }

    void *start = target - 4;

    //if it's already been removed
    if(*(uint32_t *)start == 0) {
        return;
    }

    //removing it
    size_t size = (size_t)(*(uint32_t *)start);
    for(size_t i = 0; i < size; ++i)
    {
        *(uint32_t *)(start + i) = 0;
    }
}

void *krealloc(void *target, size_t new_size) {
    //null detection
    if(target == NULL) {
        return kmalloc(new_size);
    }

    void *start = target;
    uint32_t old_size = *(uint32_t *)(start - 4);
    void *new_address = kmalloc(new_size * 4);
    if(new_address == NULL) {
        return NULL;
    }

    memmove(new_address, start, old_size < new_size ? old_size : new_size);
    kfree(target);
    return new_address;
}

void kheap_init() {
    //4 pages for the initial kernel heap
    kheap_start = allocate_continuous_pages(4);
    kheap_end = kheap_start + 4 * PAGE_SIZE;
    kheap_current_address = kheap_start;
    kprintf("kheap start: %x, kheap end: %x\n", kheap_start, kheap_end);
}