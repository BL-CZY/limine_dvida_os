#ifndef MOUNT_MANAGER_H
#define MOUNT_MANAGER_H

#include "hal/structs/hal_storage_structs.h"
#include "hal/fs/hal_fs.h"
#include "drivers/partition/gpt.h"

extern mounted_partition_t *root_partition;

// dvida os root partition guid
// d2e086bb-aee0-4632-bf7c-f10768da1d6a
static guid_t DVIDA_OS_ROOT_GUID = {
    .data1 = {0xd2, 0xe0, 0x86, 0xbb},
    .data2 = {0xae, 0xe0},
    .data3 = {0x46, 0x32},
    .data4 = {0xbf, 0x7c},
    .data5 = {0xf1, 0x07, 0x68, 0xda, 0x1d, 0x6a},
};

int chroot(storage_device_t *drive, uint16_t index);
int mount();
int umount();

#endif