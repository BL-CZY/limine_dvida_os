#include "../../lib/memory/pmm.h"
#include "../../lib/handlers/handle_utils.h"

uint8_t *bit_map_start;
int bit_map_length;

void fill_map(struct limine_memmap_response response) {
    //not available 1, available 0
    for(uint64_t i = 0; i < response.entry_count; ++i) {
        struct limine_memmap_entry entry = *(response.entries)[i];
        for(uint64_t j = 0; j < entry.length/PAGE_SIZE; ++j) {
            if(entry.type == LIMINE_MEMMAP_USABLE) {
                bit_map_start[i * j / 8] = bit_map_start[i * j / 8] | (0x1 << (7 - i * j % 8));
            } else {
                bit_map_start[i * j / 8] = bit_map_start[i * j / 8] | ((0xFF & (0x0 << (7 - i * j % 8))));
            }
        }
    }
}

void memmap_init(struct limine_memmap_response response) {
    unsigned long long int usable_length = 0;
    unsigned long long int total_length = 0;
    for(int i = 0; i < (int)response.entry_count; ++i) {
        struct limine_memmap_entry entry = *(response.entries)[i];
        printf("memory entry %x: ", i);
        printf("base: %x length: %x type: %x \n", entry.base, entry.length, entry.type);
        if(entry.type == LIMINE_MEMMAP_USABLE) {
            usable_length += entry.length;
        }
        total_length += entry.length;
    }
    printf("total usable memory: %uGib, %uMib, %uKib\n", usable_length/1024/1024/1024, (usable_length/1024/1024)%1024, (usable_length/1024)%1024);
    printf("total usable pages: %u\n", usable_length/PAGE_SIZE);
    if(usable_length/1024/1024/1024 == 0) {
        panic("no enough ram");
    }

    //initialize bitmap
    bit_map_length = total_length/PAGE_SIZE;
    int is_success = 0;
    for(int i = 0; i < (int)response.entry_count; ++i) {
        struct limine_memmap_entry entry = *(response.entries)[i];
        if((int)entry.length >= bit_map_length) {
            is_success = 1;
            bit_map_start = (uint8_t *)entry.base;
            //fill bitmap
            fill_map(response);
            break;
        }
    }
    if(!is_success) {
        panic("bit map won't fit");
    }
}