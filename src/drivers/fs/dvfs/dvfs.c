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
 * relative lba
 */
int read_single_dir(dvfs_t *fs, uint64_t lba, dvfs_dir_t *result) {
    // initialize the vector
    new_vector(sizeof(dvfs_file_t), &result->files);
    uint64_t current_lba = lba;

    while (true) {
        uint8_t buffer[fs->header.block_size * SECTOR_SIZE];
        uint32_t file_entry_num = (fs->header.block_size * SECTOR_SIZE / FILE_ENTRY_SIZE) - 1;
        read_sectors(&fs->drive, current_lba, fs->header.block_size, buffer);

        for (int i = 0; i < file_entry_num; ++i) {
            uint8_t *current_entry_start = buffer + (i * FILE_ENTRY_SIZE);

            // for every entry, check for the flag
            uint32_t flags = little_endian_to_uint32(current_entry_start + FILE_ENTRY_FLAG_OFFSET);

            // if it's a regular file
            if((flags & REG_FILE_MASK) == REG_FILE_MASK) {
                dvfs_file_t file = {
                    .type = REG,
                };

                // read in the name and extension
                for(int j = 0; j < 220; ++j) {
                    file.reg_file.name[j] = current_entry_start[j];
                }

                for(int j = 0; j < 24; ++j) {
                    file.reg_file.extension[j] = current_entry_start[220 + j];
                }

                // block address and flags
                file.reg_file.block_addr = little_endian_to_uint64(current_entry_start + 244);
                file.reg_file.flags = little_endian_to_uint32(current_entry_start + 252);

                // push to the vector
                result->files.push(&result->files, &file);
            } else if((flags & DIR_FILE_MASK) == DIR_FILE_MASK) { // if it's a dir
                dvfs_file_t file = {
                    .type = DIR,
                };

                // read in the name
                for(int j = 0; j < 244; ++j) {
                    file.dir_file.name[j] = current_entry_start[j];
                }

                // block address and flags
                file.dir_file.block_addr = little_endian_to_uint64(current_entry_start + 244);
                file.dir_file.flags = little_endian_to_uint32(current_entry_start + 252);

                // push to the vector
                result->files.push(&result->files, &file);
            }
        }

        // check for the next block
        uint64_t next_block_addr = little_endian_to_uint64(buffer + (fs->header.block_size * SECTOR_SIZE) - 12);

        // if there is no more block, return
        if(next_block_addr == 0) {
            return 0;
        } else {
            current_lba = next_block_addr;
        }
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
            bool found = false;

            for(uint32_t j = 0; j < file_entry_num; ++j) {
                uint8_t *current_entry_start = buffer + (j * FILE_ENTRY_SIZE);

                // for every entry, check for the flag
                uint32_t flags = little_endian_to_uint32(current_entry_start + FILE_ENTRY_FLAG_OFFSET);

                // if it's not a dir, ignore it
                if((flags & DIR_FILE_MASK) != DIR_FILE_MASK) {
                    continue;
                }

                // this part will run if it's a dir
                // check for the name
                if(strcmp(str, (char *)(current_entry_start))) {
                    found = true;
                    // if it's the same and it's the last element in the path
                    if(i == path->count - 1) {
                        // copy the name
                        for(int k = 0; k < 244; ++k) {
                            result->name[k] = current_entry_start[k];
                        }

                        // copy the flags
                        result->flags = flags;

                        // read the directory
                        return read_single_dir(fs, little_endian_to_uint64(current_entry_start + FILE_ENTRY_ADDR_OFFSET), result);
                    }

                    // if this is not the end of the path, end the while loop
                    break;
                }
            }

            if(found) {
                break;
            }

            // check for the next block
            uint64_t next_block_addr = little_endian_to_uint64(buffer + (fs->header.block_size * SECTOR_SIZE) - 12);

            // if we haven't found the corresponding directory and there is no more block, there is an error
            if(next_block_addr == 0) {
                return 1;
            } else {
                current_block_lba = next_block_addr;
            }
        }
    }
    return 0;
}

int read_regfile(dvfs_t *fs, vector_t *path, dvfs_reg_t *result) {
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
            bool found = false;

            for(uint32_t j = 0; j < file_entry_num; ++j) {
                uint8_t *current_entry_start = buffer + (j * FILE_ENTRY_SIZE);

                // for every entry, check for the flag
                uint32_t flags = little_endian_to_uint32(current_entry_start + FILE_ENTRY_FLAG_OFFSET);

                // if it's not a dir, ignore it
                if((flags & DIR_FILE_MASK) != DIR_FILE_MASK) {
                    continue;
                }

                // this part will run if it's a dir
                // check for the name
                if(strcmp(str, (char *)(current_entry_start))) {
                    found = true;
                    // if it's the same and it's the last element in the path
                    if(i == path->count - 1) {
                        // copy the name
                        for(int k = 0; k < 244; ++k) {
                            result->name[k] = current_entry_start[k];
                        }

                        // copy the flags
                        result->flags = flags;

                        // read the directory
                        return read_single_dir(fs, little_endian_to_uint64(current_entry_start + FILE_ENTRY_ADDR_OFFSET), result);
                    }

                    // if this is not the end of the path, end the while loop
                    break;
                }
            }

            if(found) {
                break;
            }

            // check for the next block
            uint64_t next_block_addr = little_endian_to_uint64(buffer + (fs->header.block_size * SECTOR_SIZE) - 12);

            // if we haven't found the corresponding directory and there is no more block, there is an error
            if(next_block_addr == 0) {
                return 1;
            } else {
                current_block_lba = next_block_addr;
            }
        }
    }
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