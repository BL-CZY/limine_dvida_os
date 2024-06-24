#ifndef DVFS_STRUCTS_H
#define DVFS_STRUCTS_H

#include <stdint.h>

#include "mod/data_structs/vector.h"
#include "hal/structs/hal_storage_structs.h"
#include "drivers/partition/gpt.h"

//* everything in relative lba

enum dvfs_file_type {
    REG,
    DIR,
};

typedef struct dvfs_file_reg {
    uint8_t name[220]; // utf-8
    uint8_t type[24]; // utf-8
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
    dvfs_file_t files[15];
    uint64_t next_block; // set to 0 if it's the end
    uint32_t flags;
} dvfs_block_t;

typedef struct dvfs_dir {
    vector_t dirs; // elements are dvfs_file
    uint32_t flags;
} dvfs_dir_t;

typedef struct dvfs_header {
    uint8_t signature[4]; // DVFS
    uint64_t root_lba;
    uint8_t reserved[244];
    vector_t bitmap;
} dvfs_header_t;

typedef struct dvfs {
    storage_device_t *drive;
    gpt_efi_header_t *descriptor;
    dvfs_header_t header;
    dvfs_dir_t root;
} dvfs_t;

bool identify_dvfs(storage_device_t *drive, gpt_table_entry_t *entry, dvfs_header_t *result);
int read_dir(dvfs_header_t *header, vector_t *path, dvfs_dir_t *result);

#endif