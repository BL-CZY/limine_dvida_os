#include "./dvfs_structs.h"

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
    
    dvfs_header_t header;
    dvfs_dir_t root;

    // header signature
    memmove(header.signature, buffer, 4);

    // revision
    header.revision = little_endian_to_uint32(buffer + 4);

    // root_lba (relative)
    header.root_lba = little_endian_to_uint64(buffer + 8);

    // read in the entire header
    uint8_t header_buffer[512 * header.root_lba];
    

    return 0;
}

int read_dir(dvfs_t *fs, vector_t *path, dvfs_dir_t *result) {
    return 0;
}

int read_regfile(dvfs_t *fs, vector_t *path, dvfs_regfile_content_t *result) {
    return 0;
}

int create_dir(dvfs_t *fs, vector_t *path, char **name) {
    return 0;
}

int create_regfile(dvfs_t *fs, vector_t *path, char **name, char **extension) {
    return 0;
}
 
int delete_dir(dvfs_t *fs, vector_t *path, bool recursive) {
    return 0;
}

int delete_regfile(dvfs_t *fs, vector_t *path) {
    return 0;
}