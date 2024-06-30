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

// empty slot has 0000

// type code: 0001
typedef struct dvfs_reg_fentry {
    uint8_t name[220]; // ASCII, end with a 0, so max 219
    uint8_t extension[24]; // ASCII, end with a 0, so max 23
    uint64_t block_addr;
    uint32_t flags; // the 4 least significant bits are the type bits
} dvfs_reg_fentry_t;

// type code: 0010
typedef struct dvfs_dir_fentry {
    uint8_t name[244]; // ASCII, end with a 0, so max 243
    uint64_t block_addr;
    uint32_t flags; // the 4 least significant bits are the type bits
} dvfs_dir_fentry_t;

typedef struct dvfs_file {
    enum dvfs_file_type type;
    union {
        dvfs_reg_fentry_t reg_file;
        dvfs_dir_fentry_t dir_file;
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
    uint8_t name[244];
    vector_t files; // elements are dvfs_file
    uint32_t flags;
} dvfs_dir_t;

typedef struct dvfs_reg {
    uint8_t name[220];
    uint8_t extension[24];
    vector_t content; // elements are uint8_t
    uint32_t flags;
} dvfs_reg_t;

typedef struct dvfs_header {
    uint8_t signature[4]; // DVFS
    uint32_t revision;
    uint64_t root_lba;
    uint64_t bitmap_length; // in bytes
    uint32_t block_size; // normally 8, which is 4096 bytes
    uint8_t reserved[228];
    vector_t bitmap;
} dvfs_header_t;

typedef struct dvfs {
    storage_device_t *drive;
    gpt_table_entry_t *descriptor;
    dvfs_header_t header;
} dvfs_t;

#endif