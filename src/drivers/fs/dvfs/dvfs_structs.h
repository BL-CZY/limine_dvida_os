#ifndef DVFS_STRUCTS_H
#define DVFS_STRUCTS_H

#include <stdint.h>

#include "mod/data_structs/vector.h"
#include "hal/structs/hal_storage_structs.h"
#include "drivers/partition/gpt.h"
#include "hal/storage/hal_storage.h"

//* everything in relative lba
// *every number is stored as little endian

static const uint8_t SIGNATURE[4] = { 0x44, 0x56, 0x46, 0x53 };

enum dvfs_file_type {
    REG,
    DIR,
};

typedef struct dvfs_file_reg {
    uint8_t name[220]; // utf-8
    uint8_t extension[24]; // utf-8
    uint64_t block_addr;
    uint32_t flags;
} dvfs_file_reg_t;

typedef struct dvfs_file_dir {
    uint8_t name[244]; // utf-8
    uint64_t block_addr;
    uint32_t flags;
} dvfs_file_dir_t;

typedef struct dvfs_file {
    enum dvfs_file_type type;
    union {
        dvfs_file_reg_t reg_file;
        dvfs_file_dir_t dir_file;
    };
    
} dvfs_file_t;

typedef struct dvfs_block {
    uint8_t data[3840];
    uint64_t next_block; // set to 0 if it's the end
    uint32_t flags;
} dvfs_block_t;

typedef struct dvfs_regfile_content {
    vector_t content; // elements are uint8
    uint32_t flags;
} dvfs_regfile_content_t;

typedef struct dvfs_dir {
    vector_t files; // elements are dvfs_file
    uint32_t flags;
} dvfs_dir_t;

typedef struct dvfs_header {
    uint8_t signature[4]; // DVFS
    uint32_t revision;
    uint64_t root_lba;
    uint8_t reserved[240];
    vector_t bitmap;
} dvfs_header_t;

typedef struct dvfs {
    storage_device_t *drive;
    gpt_table_entry_t *descriptor;
    dvfs_header_t header;
    dvfs_dir_t root;
} dvfs_t;

int init_dvfs(storage_device_t *drive, gpt_table_entry_t *entry);
int identify_dvfs(storage_device_t *drive, gpt_table_entry_t *entry, dvfs_t *result);
int read_dir(dvfs_t *fs, vector_t *path, dvfs_dir_t *result);
int read_regfile(dvfs_t *fs, vector_t *path, dvfs_regfile_content_t *result);
int create_dir(dvfs_t *fs, vector_t *path, char **name);
int create_regfile(dvfs_t *fs, vector_t *path, char **name, char **extension);
int delete_dir(dvfs_t *fs, vector_t *path, bool recursive);
int delete_regfile(dvfs_t *fs, vector_t *path);

#endif