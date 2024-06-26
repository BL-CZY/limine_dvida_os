#include "./dvfs.h"

int init_dvfs(storage_device_t *drive, gpt_table_entry_t *entry) {
    return 0;
}

/**
 * takes in the target drive, the partition entry the of the gpt table of the drive, and a result
 * error codes:
 * 1: dvfs signature not present
 */
int identify_dvfs(storage_device_t *drive, gpt_table_entry_t *entry, dvfs_t *result) {
    // TODO more error handling
    // read the first sector of the partition
    uint8_t buffer[512];
    read_sectors(drive, entry->start_lba, 1, buffer);

    //verify signature
    for(int i = 0; i < 4; ++i) {
        if(SIGNATURE[i] != buffer[i]) {
            // not dvfs
            return 1;
        }
    }

    result->drive = drive;
    result->descriptor = entry;
    
    // result->header signature
    memmove(result->header.signature, buffer, 4);

    // revision
    result->header.revision = little_endian_to_uint32(buffer + 4);

    // root_lba (relative)
    result->header.root_lba = little_endian_to_uint64(buffer + 8);

    // bitmap length
    result->header.bitmap_length = little_endian_to_uint64(buffer + 16);

    // reserved
    for(int i = 0; i < 232; ++i) {
        result->header.reserved[i] = 0;
    }

    // read in the entire header
    uint8_t header_buffer[512 * result->header.root_lba];

    // intialize the bitmap vector
    new_vector(1, &result->header.bitmap);

    // read the bitmap
    for(int i = 0; i < result->header.bitmap_length; ++i) {
        uint8_t temp = *(buffer + 256 + i);
        vector_push(&temp, &result->header.bitmap);
    }

    #pragma region read_root
    
    // read the root directory
    while(true) {

    }

    #pragma endregion

    return 0;
}

int read_dir(dvfs_t *fs, vector_body_t *path, dvfs_dir_t *result) {
    return 0;
}

int read_regfile(dvfs_t *fs, vector_body_t *path, dvfs_regfile_content_t *result) {
    return 0;
}

int create_dir(dvfs_t *fs, vector_body_t *path, char **name) {
    return 0;
}

int create_regfile(dvfs_t *fs, vector_body_t *path, char **name, char **extension) {
    return 0;
}
 
int delete_dir(dvfs_t *fs, vector_body_t *path, bool recursive) {
    return 0;
}

int delete_regfile(dvfs_t *fs, vector_body_t *path) {
    return 0;
}