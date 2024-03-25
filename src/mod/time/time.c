#include "../../lib/time/time.h"
#include "../../lib/std/stdio.h"

long long unsigned int global_timer = 0;
void (*update_list[256])();
int list_info[256] = {0};

void on_update () {
    for(int i = 0; i < 256; ++i) {
        if(list_info[i] == 0) {
            continue;
        }
        update_list[i]();
    }
    ++global_timer;
    printf("%u\n", global_timer);
}

int register_update_function(void (*func)()) {
    for(int i = 0; i < 256; ++i) {
        if(list_info[i] == 0) {
            update_list[i] = func;
            list_info[i] = 1;
            return i;
        }
    }
    return -1;
}

void unregister_update_function(int index) {
    list_info[index] = 0;
}