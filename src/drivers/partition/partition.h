#ifndef PARTITION_UTILS_H
#define PARTITION_UTILS_H

#include <stdbool.h>
#include "gpt.h"
#include "../ata/pata.h"

//this function checks if the gpt table is here
bool is_gpt_present(ata_drive_t *drive);

//this function creates a new gpt table
void create_gpt(ata_drive_t *drive);

//this function reads the gpt table
int read_gpt(ata_drive_t *drive, gpt_efi_header_t *result_header, gpt_table_t *result_table);

#endif