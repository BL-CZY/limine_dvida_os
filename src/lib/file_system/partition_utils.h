#ifndef PARTITION_UTILS_H
#define PARTITION_UTILS_H

#include <stdbool.h>
#include "../../mod/file_system/gpt.h"

//this function checks if the gpt table is here
bool is_gpt_present();

//this function creates a new gpt table
void create_gpt();

//this function reads the gpt table
gpt_table_t *read_gpt();

#endif