//implemented in mod/time/time.c

#ifndef TIME_H
#define TIME_H

#include <stdint.h>

extern long long unsigned int global_timer;
extern void (*update_list[256])();

void on_update();
//return -1 if not succeed
//return the index in the list if succeed
int register_update_function(void (*func)());
void unregister_update_function(int index);

void sleep(int time);

#endif