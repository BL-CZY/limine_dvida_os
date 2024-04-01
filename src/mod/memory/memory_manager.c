#include "../../lib/memory/pmm.h"
#include "../../lib/memory/vmm.h"
#include "../../lib/utils/math_utils.h"
#include "../../lib/utils/mem_utils.h"
#include "memory_manager.h"

uint8_t *bit_map_start;
int bit_map_length;

void fill_map(struct limine_memmap_response response, struct limine_hhdm_response hhdm) {
    //not available 1, available 0
    memset(bit_map_start + hhdm.offset, (int)0xff, bit_map_length);
    printf("%u", *(bit_map_start + hhdm.offset));
}

void pmm_init(struct limine_memmap_response response, struct limine_hhdm_response hhdm) {
    printf("hhdm offset: %x\n", hhdm.offset);

    unsigned long long int usable_length = 0;
    for(int i = 0; i < (int)response.entry_count; ++i) {
        struct limine_memmap_entry entry = *(response.entries)[i];
        printf("memory entry %x: ", i);
        printf("base: %x length: %x type: %x \n", entry.base, entry.length, entry.type);
        if(entry.type == LIMINE_MEMMAP_USABLE) {
            usable_length += entry.length;
        }
    }
    printf("total usable memory: %uGib, %uMib, %uKib\n", usable_length/1024/1024/1024, (usable_length/1024/1024)%1024, (usable_length/1024)%1024);
    printf("total usable pages: %u\n", usable_length/PAGE_SIZE);
    if(usable_length/1024/1024/1024 == 0) {
        panic("no enough ram");
    }

    //initialize bitmap
    bit_map_length = round_up(round_up((response.entries[response.entry_count - 1]->base + response.entries[response.entry_count - 1]->length), PAGE_SIZE), 8);
    int is_success = 0;
    for(int i = 0; i < (int)response.entry_count; ++i) {
        struct limine_memmap_entry entry = *(response.entries)[i];
        if((int)entry.length >= bit_map_length) {
            is_success = 1;
            bit_map_start = (uint8_t *)entry.base;
            //fill bitmap
            fill_map(response, hhdm);
            break;
        }
    }
    if(!is_success) {
        panic("bit map won't fit");
    }
}

void vmm_init(struct limine_memmap_response response) {
    
}
