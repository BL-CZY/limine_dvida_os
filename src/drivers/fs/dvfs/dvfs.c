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

    // block size
    result->header.block_size = little_endian_to_uint32(buffer + 24);

    // reserved
    for(int i = 0; i < 228; ++i) {
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

    return 0;
}

/**
 * takes in the filesystem, the path as a vector of char *, and a dvfs_dir_t as result
 * error codes:
 * 1: path not valid
 */
int read_dir(dvfs_t *fs, vector_t *path, dvfs_dir_t *result) {
    // init current block address
    uint64_t current_block_lba = fs->header.root_lba + fs->descriptor->start_lba;

    for(uint32_t i = 0; i < path->count; ++i) {
        // path is a vector of char *
        char *str;
        path->get(path, i, (void *)str);

        while(true) {
            // read all the files in the current block
            uint8_t buffer[fs->header.block_size * SECTOR_SIZE];
            uint32_t file_entry_num = (fs->header.block_size * SECTOR_SIZE / FILE_ENTRY_SIZE) - 1;
            read_sectors(&fs->drive, current_block_lba, fs->header.block_size, buffer);
            for(uint32_t j = 0; j < file_entry_num; ++j) {
                // for every entry, check for the flag
                uint32_t flags = little_endian_to_uint32(buffer + (j * FILE_ENTRY_SIZE) + FILE_ENTRY_FLAG_OFFSET);

                // if it's not a dir, ignore it
                if((flags & DIR_FILE_FILTER) != DIR_FILE_FILTER) {
                    continue;
                }

                // this part will run if it's a dir
            }

            // check for the next block
            uint64_t next_block_addr = little_endian_to_uint64(buffer + (fs->header.block_size * SECTOR_SIZE) - 12);

            // if we haven't found the corresponding directory and there is no more block, there is an error
            if(next_block_addr == 0) {
                return 1;
            }
        }
    }
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