#ifndef DVFS_H
#define DVFS_H

#include "./dvfs_structs.h"

#define SECTOR_SIZE 512
#define FILE_ENTRY_SIZE 256
#define DIR_FILE_FILTER 0x0010
#define REG_FILE_FILTER 0x0001
#define FILE_ENTRY_FLAG_OFFSET 252

int init_dvfs(storage_device_t *drive, gpt_table_entry_t *entry);
int identify_dvfs(storage_device_t *drive, gpt_table_entry_t *entry, dvfs_t *result);
int read_dir(dvfs_t *fs, vector_t *path, dvfs_dir_t *result);
int read_regfile(dvfs_t *fs, vector_t *path, dvfs_regfile_content_t *result);
int create_dir(dvfs_t *fs, vector_t *path, char **name);
int create_regfile(dvfs_t *fs, vector_t *path, char **name, char **extension);
int delete_dir(dvfs_t *fs, vector_t *path, bool recursive);
int delete_regfile(dvfs_t *fs, vector_t *path);

#endif