#include "./mount_manager.h"

/**
 * takes in the storage device and the index in the partition array
 * set the root directory to it
 * error codes:
 * 1: drive not identified
 * 2: no gpt table
 * 3: index too large
 * 4: the entry is not used
 * 5: partition type not ok
 */
int chroot(storage_device_t *drive, uint16_t index) {
    if(!drive->identified) {
        return 1;
    }

    if(!is_gpt_present(drive->device_gpt_header.signature)) {
        // gpt not here
        return 2;
    }

    if(index >= drive->device_gpt_header.entry_num) {
        // index too big
        return 3;
    }

    uint8_t buffer[16];
    guid_to_buffer(&drive->device_gpt_table.entries[index].partition_type_guid, buffer);
    if(is_entry_unused(buffer)) {
        // entry unused
        return 4;
    }

    if(!are_guid_same(&DVIDA_OS_ROOT_GUID, &drive->device_gpt_table.entries[index].partition_type_guid)) {
        // partition is not type
        return 5;
    }

    
}