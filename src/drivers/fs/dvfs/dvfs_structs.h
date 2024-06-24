#ifndef DVFS_STRUCTS_H
#define DVFS_STRUCTS_H

#include <stdint.h>

//* everything in relative lba

typedef struct dvfs_block {
    uint8_t content[3840]; // 15 types
    uint64_t next_block; // set to 0 if it's the end
    uint32_t flags;
} dvfs_block_t;

typedef struct dvfs_file {
    uint8_t name[220]; // utf-8
    uint8_t type[24]; // utf-8
    uint64_t block_addr;
    uint32_t flags;
} dvfs_file_t;

typedef struct dvfs_dir {
    uint8_t name[244]; // utf-8
    uint64_t block_addr;
    uint32_t flags;
};


#endif