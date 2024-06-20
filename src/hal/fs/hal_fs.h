#ifndef HAL_FS_H
#define HAL_FS_H

#include "hal/structs/hal_storage_structs.h"
#include "drivers/partition/gpt.h"

int identify_fs(storage_device_t *drive, gpt_table_entry_t *target);

#endif