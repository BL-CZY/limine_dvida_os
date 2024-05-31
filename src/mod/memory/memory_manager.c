#include "../../lib/memory/pmm.h"
#include "../../lib/utils/math_utils.h"
#include "../../lib/utils/mem_utils.h"
#include "memory_manager.h"

uint8_t *bit_map_start;
uint64_t bit_map_length;
uint64_t hhdm_offset;

uint64_t bit_map_get_bit(uint64_t num) {
    return (bit_map_start[num/8] >> (7 - num%8)) & 0x1;
}

//set to 1
void bit_map_allocate_bit(uint64_t bit) {
    bit_map_start[bit/8] = bit_map_start[bit/8] | (0x1 << (7 - bit%8));
}

//set to 0
void bit_map_free_bit(uint64_t bit) {
    bit_map_start[bit/8] &= ~(0x1 << (7 - bit%8));
}

void fill_map(struct limine_memmap_response response, uint64_t bit_map_entry) {
    //not available 1, available 0
    memset(bit_map_start, (int)0xff, bit_map_length);

    //free usable pages
    for(uint64_t i = 0; i < (uint64_t)response.entry_count; ++i) {
        struct limine_memmap_entry entry = *(response.entries)[i];

        if(response.entries[i]->type == LIMINE_MEMMAP_USABLE) {
            for(uint64_t j = 0; j < entry.length/PAGE_SIZE; ++j) {
                bit_map_free_bit((entry.base/PAGE_SIZE) + j);
            }
        }

        if(i == bit_map_entry) {
            //set the pages used for the map
            for(uint64_t j = 0; j < bit_map_length/PAGE_SIZE; ++j) {
                bit_map_allocate_bit((entry.base/PAGE_SIZE) + j);
            }
        }
    }
}

void pmm_init(struct limine_memmap_response response, struct limine_hhdm_response hhdm) {
    if(hhdm.offset == 0 || (uint64_t)response.entries < 1) {
        kpanic("no memmap or hhdm");
    }
    kprintf("hhdm offset: %x\n", hhdm.offset);
    hhdm_offset = hhdm.offset;

    //pruint64_t out the info
    uint64_t usable_length = 0;
    for(uint64_t i = 0; i < (uint64_t)response.entry_count; ++i) {
        struct limine_memmap_entry entry = *(response.entries)[i];
        kprintf("memory entry %x: ", i);
        kprintf("base: %x length: %x type: %x \n", entry.base, entry.length, entry.type);
        if(entry.type == LIMINE_MEMMAP_USABLE) {
            usable_length += entry.length;
        }
    }
    kprintf("total usable memory: %uGib, %uMib, %uKib\n", usable_length/1024/1024/1024, (usable_length/1024/1024)%1024, (usable_length/1024)%1024);
    kprintf("total usable pages: %u\n", usable_length/PAGE_SIZE);
    if(usable_length/1024/1024/1024 == 0) {
        kpanic("no enough ram");
    }

    //initialize bitmap, the length is round up always
    bit_map_length = round_up(round_up((response.entries[response.entry_count - 1]->base + response.entries[response.entry_count - 1]->length), PAGE_SIZE), 8);
    uint64_t is_success = 0;

    //find a usable chunk
    for(uint64_t i = 0; i < (uint64_t)response.entry_count; ++i) {
        struct limine_memmap_entry entry = *(response.entries)[i];
        if((uint64_t)entry.length >= bit_map_length) {
            is_success = 1;
            bit_map_start = (uint8_t *)(entry.base + hhdm.offset);
            //fill bitmap
            fill_map(response, i);
            break;
        }
    }

    if(!is_success) {
        kpanic("bit map won't fit");
    }
}

void *allocate_continuous_pages(size_t num) {

    uint64_t usable_pages_in_segment = 0;
    uint64_t start = 0;
    uint64_t end = 0;

    for(uint64_t i = 0; i < bit_map_length * 8; ++i) {
        if(bit_map_get_bit(i) == 0) {
            ++usable_pages_in_segment;
            if(usable_pages_in_segment == 1) {
                start = i; // Start of the current segment
            }
            if(usable_pages_in_segment == num) {
                end = i; // End of the current segment
                break; // Found a segment that can accommodate the requested number of pages
            }
        } else {
            usable_pages_in_segment = 0;
        }
    }

    if(usable_pages_in_segment != num) {
        kpanic("Not enough memory");
        return NULL;
    }

    // Allocate the pages by setting their bits to 1
    for(uint64_t i = start; i <= end; ++i) {
        bit_map_allocate_bit(i);
    }

    return (void *)(start * PAGE_SIZE + hhdm_offset);
}

void free_continuous_pages(void *start, uint64_t length) {
    uint64_t bit_map_index = (uint64_t)((uint8_t *)start - hhdm_offset) / PAGE_SIZE;

    for(uint64_t i = bit_map_index; i < bit_map_index + length; ++i) {
        bit_map_free_bit(i);
    }
}